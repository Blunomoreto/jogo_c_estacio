# Instalação do Orbit Siege (Windows + MinGW Standalone)

## 1 Pré-requisitos obrigatórios

- **MinGW-w64 standalone** com GCC 10+ (ex.: [WinLibs](https://winlibs.com))
- **FreeGLUT** - baixado automaticamente pelo script de setup
- **CMake 3.16+** - necessário para etapas que usam cmake ou compilar compilar FreeGLUT no setup inicial com cmake ([cmake.org](https://cmake.org/download/))
- **OpenGL** - já incluso no Windows (opengl32, glu32)

## 2 Instalar MinGW-w64 standalone

1. Baixe uma distribuição standalone de MinGW-w64 (ex.: WinLibs).
2. Extraia em um caminho sem espaços, por exemplo `C:\mingw64`.
3. Garanta que os executáveis `C:\mingw64\bin\gcc.exe` e `C:\mingw64\bin\g++.exe` existem
4. Adicione `C:\mingw64\bin` ao PATH do Windows.
5. Abra um novo PowerShell e valide:

gcc --version
cmake --version

## 3 Setup do projeto (primeira vez)

No diretório raiz do projeto, se preferir usar cmake execute:

powershell -ExecutionPolicy Bypass -File .\scripts\setup_libs_cmake.ps1

Ou, se prefirir instalação direta sem cmake utilize:

powershell -ExecutionPolicy Bypass -File .\scripts\setup_libs.ps1

Esse script:

- Valida que `gcc` e `cmake` estão no PATH
- Baixa o código-fonte do FreeGLUT 3.8.0 de [github.com/freeglut/freeglut/releases](https://github.com/freeglut/freeglut/releases)
- Compila FreeGLUT com MinGW Makefiles
- Organiza headers, lib e `libfreeglut.dll` em `src\third_party\freeglut\freeglut\`

O setup é idempotente: se `libfreeglut.dll` já existir, o script encerra imediatamente.

## 4 Compilar o jogo

### 4.1 Setup automatizado via powershell (recomendado)

No diretório raiz do projeto:

powershell -ExecutionPolicy Bypass -File .\scripts\compile_build.ps1

Esse script:

- Compila todos os arquivos `.c` de `src\lib\`
- Linka com opengl32, glu32, freeglut e winmm
- Gera `src\orbit_siege.exe`
- Copia `libfreeglut.dll` para `src\`

### 4.2 Compilação manual

A partir da raiz do projeto:

gcc -I"src\include" -I"src\third_party\freeglut\freeglut\include" src\lib\main.c src\lib\jogo.c src\lib\matematica.c src\lib\renderizar.c src\lib\desenhar.c src\lib\inimigo.c src\lib\particulas.c src\lib\projeteis.c src\lib\melhorias.c src\lib\cenario.c src\lib\interface.c src\lib\colisao.c src\lib\persistencia.c src\lib\audio.c src\lib\imagem.c src\lib\pastas.c -L"src\third_party\freeglut\freeglut\lib\x64" -o src\orbit_siege.exe -lopengl32 -lglu32 -lfreeglut -lwinmm -lm

### 4.3 Build com CMake

Requer CMake 3.16+ e FreeGLUT instalado via `setup_libs.ps1`:

cmake -S . -B build -G "MinGW Makefiles"
cmake --build build

O executável é gerado em `build\orbit_siege.exe`. `libfreeglut.dll` é copiado automaticamente via post-build command.

## 5 Execução

Execute a partir da pasta `src\` para que os caminhos de assets e dados funcionem:

cd src
.\orbit_siege.exe

## 6 Estrutura de arquivos necessária em tempo de execução

O executável deve ser rodado a partir de `src\` com a seguinte estrutura:

src\
  orbit_siege.exe
  libfreeglut.dll
  assets\
    images\background.ppm   (opcional - fallback visual sem ele)
    audio\bgm.wav            (opcional - silencioso sem ele)
    audio\shoot.wav          (opcional)
    audio\hit.wav            (opcional)
  data\                      (criado automaticamente pelo jogo)
  screenshots\               (criado automaticamente pelo jogo)

## 7 Troubleshooting

### `gcc` não encontrado

Verifique se `C:\mingw64\bin` está no PATH e reabra o terminal.

### `cmake` não encontrado

Instale o CMake de [cmake.org/download](https://cmake.org/download) e reabra o terminal.

### `cannot find -lfreeglut`

Execute `scripts\setup_libs.ps1` para baixar e compilar FreeGLUT.
Confirme que `src\third_party\freeglut\freeglut\lib\x64\libfreeglut.a` existe.

### `fatal error: GL/freeglut.h: No such file or directory`

Execute `scripts\setup_libs.ps1`. O header deve estar em
`src\third_party\freeglut\freeglut\include\GL\freeglut.h`.

### Executável abre e fecha imediatamente

Garanta que `libfreeglut.dll` está em `src\` (ao lado do `.exe`).
O script `compile_build.ps1` faz essa cópia automaticamente.

## 8 Validação mínima pós-instalação

Após iniciar o jogo, confirme:

- Menu com START, OPTIONS, SCOREBOARD
- Movimento com teclado e tiro com mouse
- HUD com tempo, score e vida
- Pausa e tela de upgrades funcionando
- Screenshot com F12 gerando arquivo em `screenshots\`
- Salvamento de score no final da run
