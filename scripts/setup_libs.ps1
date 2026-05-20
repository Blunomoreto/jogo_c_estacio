param(
    [string]$ProjectRoot = "$(Split-Path -Parent $PSScriptRoot)\src",
    [switch]$RunSmokeTest
)

$ErrorActionPreference = "Stop"

Write-Host "[1/?] Validando dependencias..."
$gccCmd = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gccCmd) { throw "gcc nao encontrado no PATH. Instale MinGW-w64 e reabra o terminal." }
gcc --version | Select-Object -First 1 | Out-Host

$thirdParty = Join-Path $ProjectRoot "third_party"

$gccPath      = $gccCmd.Source
$isMsys2Ucrt  = $gccPath -match '[\\/]ucrt64[\\/]'
$isMsys2Mingw = $gccPath -match '[\\/]mingw64[\\/]'
$isMsys2      = $isMsys2Ucrt -or $isMsys2Mingw

if ($isMsys2) {
    $msys2Root = $gccPath -replace '[\\/](ucrt64|mingw64)[\\/].*', ''
    $pacman    = Join-Path $msys2Root "usr\bin\pacman.exe"
    if (-not (Test-Path $pacman)) {
        throw "MSYS2 detectado mas pacman nao encontrado em $pacman."
    }

    $pkg = if ($isMsys2Ucrt) { "mingw-w64-ucrt-x86_64-freeglut" } else { "mingw-w64-x86_64-freeglut" }
    Write-Host "[2/2] MSYS2 detectado - garantindo $pkg via pacman..."
    & $pacman -S --noconfirm --needed $pkg
    if ($LASTEXITCODE -ne 0) { throw "pacman falhou ao instalar $pkg." }
    Write-Host "FreeGLUT disponivel em $msys2Root."
} else {
    $freeglutBase = Join-Path $thirdParty "freeglut\freeglut"

    $existingDll = Get-ChildItem -Path (Join-Path $freeglutBase "bin\x64") -Filter "*freeglut*.dll" `
                       -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($existingDll) {
        Write-Host "FreeGLUT ja instalado em $freeglutBase - pulando setup."
    } else {
        $zipPath     = Join-Path $thirdParty "freeglut-mingw.zip"
        $extractTemp = Join-Path $thirdParty "freeglut-temp"

        $urls = @(
            "https://www.songho.ca/opengl/files/freeglut-mingw-3.8.0.zip",
            "https://github.com/freeglut/freeglut/releases/download/v3.6.0/freeglut-MinGW-3.6.0-1.mp.zip"
        )
        $baixado = $false
        Write-Host "[2/3] Baixando FreeGLUT (MinGW pre-compilado)..."
        foreach ($uri in $urls) {
            try {
                Write-Host "  Tentando: $uri"
                Invoke-WebRequest -Uri $uri -OutFile $zipPath -UseBasicParsing -ErrorAction Stop
                $baixado = $true
                break
            } catch {
                Write-Host "  Falhou: $_"
            }
        }
        if (-not $baixado) {
            throw "Nenhuma URL de download do FreeGLUT funcionou. Verifique sua conexao com a internet."
        }

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
                     Select-Object -First 1 | ForEach-Object { $_.DirectoryName }
        if (-not $headerDir) { throw "Headers do FreeGLUT nao encontrados no zip extraido." }
        Copy-Item -Force "$headerDir\*" (Join-Path $freeglutBase "include\GL\")

        Write-Host "FreeGLUT instalado em: $freeglutBase"
    }
}

$stbDir   = Join-Path $thirdParty "stb"
$stbImage = Join-Path $stbDir "stb_image.h"
$stbWrite = Join-Path $stbDir "stb_image_write.h"

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
