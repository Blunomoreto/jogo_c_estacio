param(
    [string]$ProjectRoot = "$(Split-Path -Parent $PSScriptRoot)\src",
    [switch]$RunSmokeTest
)

$ErrorActionPreference = "Stop"

Write-Host "[1/4] Validando dependencias..."
$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) {
    throw "gcc nao encontrado no PATH. Instale MinGW-w64 standalone e reabra o terminal."
}
$cmakeCmd = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmakeCmd) {
    throw "cmake nao encontrado no PATH. Instale CMake (cmake.org) e reabra o terminal."
}
gcc --version | Select-Object -First 1 | Out-Host

$thirdParty   = Join-Path $ProjectRoot "third_party"
$srcDir       = Join-Path $thirdParty "freeglut-src"
$buildDir     = Join-Path $thirdParty "freeglut-build"
$freeglutBase = Join-Path $thirdParty "freeglut\freeglut"
$tarball      = Join-Path $thirdParty "freeglut-3.8.0.tar.gz"

if (Test-Path (Join-Path $freeglutBase "bin\x64\libfreeglut.dll")) {
    Write-Host "FreeGLUT ja instalado em $freeglutBase - pulando setup."
    Copy-Item -Force (Join-Path $freeglutBase "bin\x64\libfreeglut.dll") (Join-Path $ProjectRoot "libfreeglut.dll")
    exit 0
}

Write-Host "[2/4] Baixando FreeGLUT 3.8.0..."
New-Item -ItemType Directory -Force -Path $thirdParty | Out-Null
Invoke-WebRequest `
    -Uri "https://github.com/freeglut/freeglut/releases/download/v3.8.0/freeglut-3.8.0.tar.gz" `
    -OutFile $tarball

Write-Host "[3/4] Extraindo e compilando FreeGLUT..."
New-Item -ItemType Directory -Force -Path $srcDir | Out-Null
tar -xzf $tarball -C $srcDir --strip-components=1

& cmake -S $srcDir -B $buildDir `
    -G "MinGW Makefiles" `
    -DCMAKE_BUILD_TYPE=Release `
    -DBUILD_SHARED_LIBS=ON `
    -DFREEGLUT_BUILD_DEMOS=OFF `
    -DFREEGLUT_BUILD_STATIC_LIBS=OFF
if ($LASTEXITCODE -ne 0) { throw "cmake configure falhou." }

& cmake --build $buildDir
if ($LASTEXITCODE -ne 0) { throw "cmake build falhou." }

Write-Host "[4/4] Organizando arquivos em $freeglutBase..."
New-Item -ItemType Directory -Force -Path (Join-Path $freeglutBase "bin\x64")    | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $freeglutBase "lib\x64")    | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $freeglutBase "include\GL") | Out-Null

Copy-Item -Force (Join-Path $srcDir "include\GL\*") (Join-Path $freeglutBase "include\GL\")

$builtDll = Get-ChildItem -Path $buildDir -Filter "*freeglut*.dll" -Recurse | Select-Object -First 1
if (-not $builtDll) { throw "libfreeglut.dll nao encontrado no diretorio de build." }
Copy-Item -Force $builtDll.FullName (Join-Path $freeglutBase "bin\x64\libfreeglut.dll")
Copy-Item -Force $builtDll.FullName (Join-Path $freeglutBase "bin\libfreeglut.dll")

$builtLib = Get-ChildItem -Path $buildDir -Filter "libfreeglut.dll.a" -Recurse | Select-Object -First 1
if (-not $builtLib) { throw "libfreeglut.dll.a nao encontrada no diretorio de build." }
Copy-Item -Force $builtLib.FullName (Join-Path $freeglutBase "lib\x64\libfreeglut.a")
Copy-Item -Force $builtLib.FullName (Join-Path $freeglutBase "lib\libfreeglut.a")

Copy-Item -Force (Join-Path $freeglutBase "bin\x64\libfreeglut.dll") (Join-Path $ProjectRoot "libfreeglut.dll")

Write-Host "FreeGLUT instalado em: $freeglutBase"
Write-Host "Execute scripts\compile_build.ps1 para compilar o jogo."
