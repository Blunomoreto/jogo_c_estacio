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

$existingDll = Get-ChildItem -Path (Join-Path $freeglutBase "bin\x64") -Filter "*freeglut*.dll" -ErrorAction SilentlyContinue | Select-Object -First 1
if ($existingDll) {
    Write-Host "FreeGLUT ja instalado em $freeglutBase - pulando setup."
    Copy-Item -Force $existingDll.FullName (Join-Path $ProjectRoot $existingDll.Name)
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

$builtDlls = Get-ChildItem -Path (Join-Path $freeglutBase "bin\x64") -Filter "*freeglut*.dll"
foreach ($d in $builtDlls) {
    Copy-Item -Force $d.FullName (Join-Path $ProjectRoot $d.Name)
}

Write-Host "FreeGLUT instalado em: $freeglutBase"

$vulkanInclude = Join-Path $thirdParty "vulkan\include"
$vulkanHeader  = Join-Path $vulkanInclude "vulkan\vulkan.h"
if (Test-Path $vulkanHeader) {
    Write-Host "Vulkan headers ja presentes em $vulkanInclude - pulando download."
} else {
    Write-Host "Baixando Vulkan-Headers (apenas headers; vulkan-1.dll ja vem com o driver)..."
    $vulkanZip   = Join-Path $thirdParty "vulkan-headers.zip"
    $vulkanTemp  = Join-Path $thirdParty "vulkan-temp"
    $vulkanUrls  = @(
        "https://github.com/KhronosGroup/Vulkan-Headers/archive/refs/tags/v1.3.280.zip",
        "https://github.com/KhronosGroup/Vulkan-Headers/archive/refs/tags/v1.3.275.zip"
    )
    $vkBaixou = $false
    foreach ($uri in $vulkanUrls) {
        try { Invoke-WebRequest -Uri $uri -OutFile $vulkanZip -UseBasicParsing -ErrorAction Stop; $vkBaixou = $true; break } catch { }
    }
    if ($vkBaixou) {
        Remove-Item -Recurse -Force $vulkanTemp -ErrorAction SilentlyContinue
        New-Item -ItemType Directory -Force -Path $vulkanTemp | Out-Null
        Expand-Archive -Path $vulkanZip -DestinationPath $vulkanTemp -Force
        $extractedRoot = Get-ChildItem -Path $vulkanTemp -Directory | Select-Object -First 1
        if ($extractedRoot -and (Test-Path (Join-Path $extractedRoot.FullName "include\vulkan\vulkan.h"))) {
            New-Item -ItemType Directory -Force -Path $vulkanInclude | Out-Null
            Copy-Item -Recurse -Force (Join-Path $extractedRoot.FullName "include\*") $vulkanInclude
            Write-Host "Vulkan headers instalados em $vulkanInclude"
        }
        Remove-Item -Recurse -Force $vulkanTemp -ErrorAction SilentlyContinue
        Remove-Item -Force $vulkanZip -ErrorAction SilentlyContinue
    } else {
        Write-Host "AVISO: download de Vulkan-Headers falhou. O jogo usara o fallback CPU."
    }
}

$stbDir    = Join-Path $thirdParty "stb"
$stbImage  = Join-Path $stbDir "stb_image.h"
$stbWrite  = Join-Path $stbDir "stb_image_write.h"

if ((Test-Path $stbImage) -and (Test-Path $stbWrite)) {
    Write-Host "STB headers ja presentes em $stbDir - pulando download."
} else {
    Write-Host "Baixando STB headers..."
    New-Item -ItemType Directory -Force -Path $stbDir | Out-Null
    $stbBase = "https://raw.githubusercontent.com/nothings/stb/master"
    try {
        Invoke-WebRequest -Uri "$stbBase/stb_image.h"       -OutFile $stbImage -UseBasicParsing -ErrorAction Stop
        Invoke-WebRequest -Uri "$stbBase/stb_image_write.h" -OutFile $stbWrite -UseBasicParsing -ErrorAction Stop
        Write-Host "STB headers instalados em $stbDir"
    } catch {
        throw "Falha ao baixar STB headers: $_"
    }
}

Write-Host "Execute scripts\compile_build.ps1 para compilar o jogo."
