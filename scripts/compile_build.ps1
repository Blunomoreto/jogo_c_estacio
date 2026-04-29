param(
    [string]$ProjectRoot = "$(Split-Path -Parent $PSScriptRoot)\src",
    [switch]$RunSmokeTest
)

$ErrorActionPreference = "Stop"

Write-Host "[1/2] Validando GCC..."
$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) {
    throw "gcc não encontrado no PATH. Instale MinGW-w64 standalone e reabra o terminal."
}

gcc --version | Select-Object -First 1 | Out-Host

$thirdParty = Join-Path $ProjectRoot "third_party"
$extractPath = Join-Path $thirdParty "freeglut"

$includeDir = Join-Path $ProjectRoot "include"
$glutInclude = Join-Path $extractPath "freeglut\include"
$glutLib = Join-Path $extractPath "freeglut\lib\x64"
$glutDll = Join-Path $extractPath "freeglut\bin\x64\freeglut.dll"
$outExe = Join-Path $ProjectRoot "orbit_siege.exe"
$outDll = Join-Path $ProjectRoot "freeglut.dll"

Write-Host "[2/2] Compilando jogo..."
& gcc `
    -I"$includeDir" `
    -I"$glutInclude" `
    (Join-Path $ProjectRoot "lib\main.c") `
    (Join-Path $ProjectRoot "lib\jogo.c") `
    (Join-Path $ProjectRoot "lib\matematica.c") `
    (Join-Path $ProjectRoot "lib\renderizar.c") `
    (Join-Path $ProjectRoot "lib\inimigo.c") `
    (Join-Path $ProjectRoot "lib\particulas.c") `
    (Join-Path $ProjectRoot "lib\projeteis.c") `
    (Join-Path $ProjectRoot "lib\melhorias.c") `
    (Join-Path $ProjectRoot "lib\cenario.c") `
    (Join-Path $ProjectRoot "lib\interface.c") `
    (Join-Path $ProjectRoot "lib\colisao.c") `
    (Join-Path $ProjectRoot "lib\persistencia.c") `
    (Join-Path $ProjectRoot "lib\audio.c") `
    (Join-Path $ProjectRoot "lib\print.c") `
    (Join-Path $ProjectRoot "lib\imagem.c") `
    (Join-Path $ProjectRoot "lib\pastas.c") `
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
