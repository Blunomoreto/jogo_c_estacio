param(
    [string]$ProjectRoot = "$(Split-Path -Parent $PSScriptRoot)\src",
    [switch]$RunSmokeTest
)

$ErrorActionPreference = "Stop"

Write-Host "[1/2] Validando GCC..."
$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) {
    throw "gcc nao encontrado no PATH. Instale MinGW-w64 standalone e reabra o terminal."
}

gcc --version | Select-Object -First 1 | Out-Host

$thirdParty  = Join-Path $ProjectRoot "third_party"
$extractPath = Join-Path $thirdParty "freeglut"

$includeDir  = Join-Path $ProjectRoot "include"
$glutInclude = Join-Path $extractPath "freeglut\include"
$glutLib     = Join-Path $extractPath "freeglut\lib\x64"
$glutDll     = Join-Path $extractPath "freeglut\bin\x64\libfreeglut.dll"
$outExe      = Join-Path $ProjectRoot "orbit_siege.exe"
$outDll      = Join-Path $ProjectRoot "libfreeglut.dll"

if (-not (Test-Path $glutDll)) {
    throw "freeglut.dll nao encontrado em $glutDll. Execute scripts\setup_libs.ps1 primeiro."
}

Write-Host "[2/2] Compilando jogo..."
& gcc `
    -I"$includeDir" `
    -I"$glutInclude" `
    (Join-Path $ProjectRoot "lib\main.c") `
    (Join-Path $ProjectRoot "lib\jogo.c") `
    (Join-Path $ProjectRoot "lib\matematica.c") `
    (Join-Path $ProjectRoot "lib\renderizar.c") `
    (Join-Path $ProjectRoot "lib\desenhar.c") `
    (Join-Path $ProjectRoot "lib\inimigo.c") `
    (Join-Path $ProjectRoot "lib\particulas.c") `
    (Join-Path $ProjectRoot "lib\projeteis.c") `
    (Join-Path $ProjectRoot "lib\melhorias.c") `
    (Join-Path $ProjectRoot "lib\cenario.c") `
    (Join-Path $ProjectRoot "lib\interface.c") `
    (Join-Path $ProjectRoot "lib\colisao.c") `
    (Join-Path $ProjectRoot "lib\persistencia.c") `
    (Join-Path $ProjectRoot "lib\audio.c") `
    (Join-Path $ProjectRoot "lib\imagem.c") `
    (Join-Path $ProjectRoot "lib\pastas.c") `
    -L"$glutLib" `
    -o "$outExe" `
    -lopengl32 -lglu32 -lfreeglut -lwinmm -lm
if ($LASTEXITCODE -ne 0) { throw "Compilacao falhou." }

Copy-Item -Force $glutDll $outDll
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
