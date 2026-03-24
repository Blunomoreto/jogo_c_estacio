# Instalação do Orbit Siege (Windows + MinGW Standalone)

Este guia é para **MinGW-w64 standalone** (sem MSYS2).

## 1) Pré-requisitos obrigatórios

- **MinGW-w64 standalone** (GCC + G++)
- **FreeGLUT** (headers + `.a`/`.dll.a` + `freeglut.dll`)
- **OpenGL** (já incluso no Windows: `opengl32` e `glu32`)
- **Opcional:** CMake

## 2) Instalar MinGW-w64 standalone

1. Baixe uma distribuição standalone de MinGW-w64 (ex.: WinLibs).
2. Extraia em um caminho sem espaços, por exemplo:

	- `C:\mingw64`

3. Garanta que estes executáveis existam:

	- `C:\mingw64\bin\gcc.exe`
	- `C:\mingw64\bin\g++.exe`

4. Adicione ao `PATH` do Windows:

	- `C:\mingw64\bin`

5. Abra um novo PowerShell e valide:

```powershell
gcc --version
```

## 3) Instalar FreeGLUT no MinGW standalone

Você precisa de:

- `GL/freeglut.h`
- `libfreeglut.a` (ou `libfreeglut.dll.a`)
- `freeglut.dll`

Copie para:

- Header: `C:\mingw64\include\GL\freeglut.h`
- Lib: `C:\mingw64\lib\libfreeglut.a` (ou `.dll.a`)
- DLL runtime: `C:\mingw64\bin\freeglut.dll`

> Se sua lib tiver outro nome, mantenha um alias compatível com `-lfreeglut`.

## 4) Compilar o jogo

### 4.1 Setup automatizado (recomendado)

No diretório raiz do projeto:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\setup_mingw_standalone.ps1 -RunSmokeTest
```

Esse script:

- baixa e extrai FreeGLUT MinGW em `third_party/`
- compila `orbit_siege.exe`
- copia `freeglut.dll` para a raiz
- executa smoke test opcional

### 4.2 Compilação manual (alternativa)

```powershell
gcc -Iinclude -Ithird_party\freeglut\freeglut\include src\main.c src\game.c src\collision.c src\persistence.c src\audio_winmm.c src\screenshot.c src\image.c -Lthird_party\freeglut\freeglut\lib\x64 -o orbit_siege.exe -lopengl32 -lglu32 -lfreeglut -lwinmm -lm
Copy-Item -Force third_party\freeglut\freeglut\bin\x64\freeglut.dll .\freeglut.dll
```

Se quiser usar CMake + MinGW Makefiles:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

Executáveis esperados:

- GCC direto: `orbit_siege.exe`
- CMake: `build\orbit_siege.exe`

## 5) Execução

Execute no diretório do projeto:

- `./orbit_siege.exe` (ou `build\orbit_siege.exe`)

## 6) Estrutura de arquivos necessária

Mantenha a estrutura:

- `assets/images/background.ppm` (opcional)
- `assets/audio/bgm.wav` (opcional)
- `assets/audio/shoot.wav` (opcional)
- `assets/audio/hit.wav` (opcional)
- `data/stats.dat`
- `data/scoreboard.dat`
- `data/settings.dat`
- `screenshots/`

Sem assets de áudio/imagem o jogo roda com fallback.

## 7) Troubleshooting (MinGW standalone)

### `cannot find -lfreeglut`

O linker não encontrou a biblioteca.

Verifique:

- `C:\mingw64\lib\libfreeglut.a` (ou `libfreeglut.dll.a`)
- `gcc -print-search-dirs` inclui `C:\mingw64\lib`

### `fatal error: GL/freeglut.h: No such file or directory`

- Falta header em `C:\mingw64\include\GL\freeglut.h`

### Executável abre e fecha / erro de DLL

- Garanta `freeglut.dll` em:
  - `C:\mingw64\bin`, e
  - `C:\mingw64\bin` no `PATH`
  - (ou copie `freeglut.dll` para a pasta do `.exe`)

### `cmake` não reconhecido

- Instale CMake e reabra o terminal.

## 8) Validação mínima pós-instalação

Após iniciar o jogo, confirme:

- Menu com `START`, `OPTIONS`, `SCOREBOARD`
- Movimento (teclado) e tiro (mouse)
- HUD com tempo/score/vida
- Pausa e upgrades funcionando
- Screenshot com `F12` gerando arquivo em `screenshots/`
- Salvamento de score no final da run
