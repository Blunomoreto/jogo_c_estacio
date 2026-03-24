param(
    [string]$ProjectRoot = "$(Split-Path -Parent $PSScriptRoot)",
    [switch]$RunSmokeTest
)

$ErrorActionPreference = "Stop"

Write-Host "[1/4] Validando GCC..."
$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) {
    throw "gcc não encontrado no PATH. Instale MinGW-w64 standalone e reabra o terminal."
}

gcc --version | Select-Object -First 1 | Out-Host

$thirdParty = Join-Path $ProjectRoot "third_party"
$zipPath = Join-Path $thirdParty "freeglut-MinGW.zip"
$extractPath = Join-Path $thirdParty "freeglut"

Write-Host "[2/4] Baixando FreeGLUT MinGW..."
New-Item -ItemType Directory -Force -Path $thirdParty | Out-Null
Invoke-WebRequest -Uri "https://www.transmissionzero.co.uk/files/software/development/GLUT/freeglut-MinGW.zip" -OutFile $zipPath

Write-Host "[3/4] Extraindo FreeGLUT..."
Expand-Archive -Path $zipPath -DestinationPath $extractPath -Force

$includeDir = Join-Path $ProjectRoot "include"
$glutInclude = Join-Path $extractPath "freeglut\include"
$glutLib = Join-Path $extractPath "freeglut\lib\x64"
$glutDll = Join-Path $extractPath "freeglut\bin\x64\freeglut.dll"
$outExe = Join-Path $ProjectRoot "orbit_siege.exe"
$outDll = Join-Path $ProjectRoot "freeglut.dll"

Write-Host "[4/4] Compilando jogo..."
& gcc `
    -I"$includeDir" `
    -I"$glutInclude" `
    (Join-Path $ProjectRoot "src\main.c") `
    (Join-Path $ProjectRoot "src\game.c") `
    (Join-Path $ProjectRoot "src\collision.c") `
    (Join-Path $ProjectRoot "src\persistence.c") `
    (Join-Path $ProjectRoot "src\audio_winmm.c") `
    (Join-Path $ProjectRoot "src\screenshot.c") `
    (Join-Path $ProjectRoot "src\image.c") `
    -L"$glutLib" `
    -o "$outExe" `
    -lopengl32 -lglu32 -lfreeglut -lwinmm -lm

Copy-Item -Force $glutDll $outDll
Write-Host "Setup concluído: $outExe"

if ($RunSmokeTest) {
    Write-Host "Executando smoke test..."
    $p = Start-Process -FilePath $outExe -WorkingDirectory $ProjectRoot -PassThru
    Start-Sleep -Seconds 2
    if (-not $p.HasExited) {
        Stop-Process -Id $p.Id -Force
        Write-Host "Smoke test OK (aplicação iniciou)."
    } else {
        Write-Host "Aplicação encerrou rapidamente com código $($p.ExitCode)."
    }
}
