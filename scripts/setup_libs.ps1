param(
    [string]$ProjectRoot = "$(Split-Path -Parent $PSScriptRoot)\src",
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

Write-Host "[1/2] Baixando FreeGLUT MinGW..."
New-Item -ItemType Directory -Force -Path $thirdParty | Out-Null
Invoke-WebRequest -Uri "https://www.transmissionzero.co.uk/files/software/development/GLUT/freeglut-MinGW.zip" -OutFile $zipPath

Write-Host "[2/2] Extraindo FreeGLUT..."
Expand-Archive -Path $zipPath -DestinationPath $extractPath -Force
