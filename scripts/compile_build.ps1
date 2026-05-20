param(
    [string]$ProjectRoot = "$(Split-Path -Parent $PSScriptRoot)\src",
    [switch]$RunSmokeTest
)

$ErrorActionPreference = "Stop"

Write-Host "[1/2] Validando GCC..."
$gccCmd = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gccCmd) {
    throw "gcc nao encontrado no PATH. Instale MinGW-w64 e reabra o terminal."
}
gcc --version | Select-Object -First 1 | Out-Host

$thirdParty = Join-Path $ProjectRoot "third_party"
$includeDir = Join-Path $ProjectRoot "include"
$stbInclude = Join-Path $thirdParty "stb"
$outExe     = Join-Path $ProjectRoot "orbit_siege.exe"

$gccPath      = $gccCmd.Source
$isMsys2Ucrt  = $gccPath -match '[\\/]ucrt64[\\/]'
$isMsys2Mingw = $gccPath -match '[\\/]mingw64[\\/]'
$isMsys2      = $isMsys2Ucrt -or $isMsys2Mingw

if ($isMsys2) {
    $msys2Root  = $gccPath -replace '[\\/](ucrt64|mingw64)[\\/].*', ''
    $mingwEnv   = if ($isMsys2Ucrt) { Join-Path $msys2Root "ucrt64" } else { Join-Path $msys2Root "mingw64" }
    $glutLib    = $null
    $glutInclude = $null
    $glutDlls   = Get-ChildItem -Path (Join-Path $mingwEnv "bin") -Filter "*freeglut*.dll" `
                      -ErrorAction SilentlyContinue | Where-Object { $_.Name -notlike "*.dll.a" }
    if (-not $glutDlls) {
        throw "FreeGLUT nao encontrado em $mingwEnv\bin. Execute scripts\setup_libs.ps1 primeiro."
    }
} else {
    $extractPath = Join-Path $thirdParty "freeglut"
    $glutInclude = Join-Path $extractPath "freeglut\include"
    $glutLib     = Join-Path $extractPath "freeglut\lib\x64"
    $glutDlls    = Get-ChildItem -Path (Join-Path $extractPath "freeglut\bin\x64") `
                       -Filter "*freeglut*.dll" -ErrorAction SilentlyContinue
    if (-not $glutDlls) {
        throw "FreeGLUT nao encontrado em $extractPath\freeglut\bin\x64. Execute scripts\setup_libs.ps1 primeiro."
    }
}

$stbImageH = Join-Path $stbInclude "stb_image.h"
$stbWriteH = Join-Path $stbInclude "stb_image_write.h"
if (-not (Test-Path $stbImageH) -or -not (Test-Path $stbWriteH)) {
    throw "STB headers nao encontrados em $stbInclude. Execute scripts\setup_libs.ps1 primeiro."
}

$sourceFiles = @(
    "lib\main.c", "lib\jogo.c", "lib\gameplay.c", "lib\entrada.c",
    "lib\matematica.c", "lib\renderizar.c", "lib\desenhar.c", "lib\inimigo.c",
    "lib\particulas.c", "lib\projeteis.c", "lib\melhorias.c", "lib\cenario.c",
    "lib\interface.c", "lib\colisao.c", "lib\persistencia.c",
    "lib\audio.c", "lib\imagem.c", "lib\imagem_stb.c", "lib\pastas.c"
) | ForEach-Object { Join-Path $ProjectRoot $_ }

Write-Host "[2/2] Compilando jogo..."

$gccArgs  = @("-I$includeDir", "-I$stbInclude")
if ($glutInclude) { $gccArgs += "-I$glutInclude" }
$gccArgs += $sourceFiles
if ($glutLib)     { $gccArgs += "-L$glutLib" }
$gccArgs += @("-o", $outExe, "-lopengl32", "-lglu32", "-lfreeglut", "-lwinmm", "-lm")

& gcc @gccArgs
if ($LASTEXITCODE -ne 0) { throw "Compilacao falhou." }

foreach ($dll in $glutDlls) {
    Copy-Item -Force $dll.FullName (Join-Path $ProjectRoot $dll.Name)
}
Write-Host "Compilacao concluida: $outExe"

if ($RunSmokeTest) {
    Write-Host "Executando smoke test..."
    $p = Start-Process -FilePath $outExe -WorkingDirectory $ProjectRoot -PassThru
    Start-Sleep -Seconds 2
    if (-not $p.HasExited) {
        Stop-Process -Id $p.Id -Force
        Write-Host "Smoke test OK (aplicacao iniciou)."
    } else {
        Write-Host "Aplicacao encerrou rapidamente com codigo $($p.ExitCode)."
    }
}
