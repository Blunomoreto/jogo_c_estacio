param(
    [string]$ProjectRoot = "$(Split-Path -Parent $PSScriptRoot)\src",
    [switch]$RunSmokeTest
)

$ErrorActionPreference = "Stop"

Write-Host "[1/3] Validando dependencias..."
$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) { throw "gcc nao encontrado no PATH. Instale MinGW-w64 e reabra o terminal." }
gcc --version | Select-Object -First 1 | Out-Host

$thirdParty   = Join-Path $ProjectRoot "third_party"
$freeglutBase = Join-Path $thirdParty "freeglut\freeglut"
$zipPath      = Join-Path $thirdParty "freeglut-mingw.zip"
$extractTemp  = Join-Path $thirdParty "freeglut-temp"

if (Test-Path (Join-Path $freeglutBase "bin\x64\libfreeglut.dll")) {
    Write-Host "FreeGLUT ja instalado em $freeglutBase - pulando setup."
    Copy-Item -Force (Join-Path $freeglutBase "bin\x64\libfreeglut.dll") (Join-Path $ProjectRoot "libfreeglut.dll")
    exit 0
}

Write-Host "[2/3] Baixando FreeGLUT 3.8.0 (MinGW pre-compilado)..."
New-Item -ItemType Directory -Force -Path $thirdParty | Out-Null
Invoke-WebRequest `
    -Uri "https://www.songho.ca/opengl/files/freeglut-mingw-3.8.0.zip" `
    -OutFile $zipPath

Write-Host "[3/3] Extraindo e organizando arquivos..."
Remove-Item -Recurse -Force $extractTemp -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path $extractTemp | Out-Null
Expand-Archive -Path $zipPath -DestinationPath $extractTemp -Force

New-Item -ItemType Directory -Force -Path (Join-Path $freeglutBase "bin\x64")    | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $freeglutBase "lib\x64")    | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $freeglutBase "include\GL") | Out-Null

$dll = Get-ChildItem -Path $extractTemp -Filter "libfreeglut.dll" -Recurse | Select-Object -First 1
if (-not $dll) { throw "libfreeglut.dll nao encontrada no zip extraido." }
Copy-Item -Force $dll.FullName (Join-Path $freeglutBase "bin\x64\libfreeglut.dll")
Copy-Item -Force $dll.FullName (Join-Path $freeglutBase "bin\libfreeglut.dll")

$lib = Get-ChildItem -Path $extractTemp -Filter "libfreeglut.dll.a" -Recurse | Select-Object -First 1
if (-not $lib) {
    $lib = Get-ChildItem -Path $extractTemp -Filter "libfreeglut.a" -Recurse |
           Where-Object { $_.Name -notlike "*static*" } | Select-Object -First 1
}
if (-not $lib) { throw "Import library do FreeGLUT nao encontrada no zip extraido." }
Copy-Item -Force $lib.FullName (Join-Path $freeglutBase "lib\x64\libfreeglut.a")
Copy-Item -Force $lib.FullName (Join-Path $freeglutBase "lib\libfreeglut.a")

$headerDir = Get-ChildItem -Path $extractTemp -Filter "freeglut.h" -Recurse |
             Select-Object -First 1 |
             ForEach-Object { $_.DirectoryName }
if (-not $headerDir) { throw "Headers do FreeGLUT nao encontrados no zip extraido." }
Copy-Item -Force "$headerDir\*" (Join-Path $freeglutBase "include\GL\")

Copy-Item -Force (Join-Path $freeglutBase "bin\x64\libfreeglut.dll") (Join-Path $ProjectRoot "libfreeglut.dll")

Write-Host "FreeGLUT instalado em: $freeglutBase"
Write-Host "Execute scripts\compile_build.ps1 para compilar o jogo."
