# Dicas e Truques de VSCode para o Projeto Orbit Siege

Este guia é para pessoas que nunca abriram este projeto antes. Mostra como
navegar e editar o código em **Visual Studio Code** sem se perder. Todos os
atalhos abaixo são para Windows (no macOS troque `Ctrl` por `Cmd`).

> Resumo de um minuto: aperte **Ctrl+P** para abrir um arquivo pelo nome,
> **Ctrl+Shift+F** para buscar texto em todo o projeto, **F12** para ir até a
> definição da função sob o cursor, e **passe o mouse** sobre qualquer função
> para ver seus argumentos.

## 1. Estrutura do projeto em 30 segundos

```text
jogo_c_estacio/
├── src/
│   ├── include/        ← cabeçalhos (.h) — declarações de tipos e funções
│   ├── lib/            ← implementações (.c) — a lógica do jogo de fato
│   ├── assets/         ← imagens, áudio, dados salvos
│   └── third_party/    ← bibliotecas externas (stb, freeglut)
├── scripts/            ← scripts PowerShell de setup e build
├── CMakeLists.txt      ← configuração de build via CMake
├── README.md           ← visão geral do projeto
└── DICAS_VSCODE.md     ← este arquivo
```

Regra prática: **se você quer mudar como algo funciona, edite um `.c` em
`src/lib/`**. Se você quer adicionar uma nova função, declare em um `.h` em
`src/include/` e implemente no `.c` correspondente.

## 2. Os 5 atalhos que você vai usar o tempo todo

| Atalho                | O que faz                                                | Quando usar                                                  |
| **Ctrl+P**            | Abrir arquivo pelo nome (digite uma parte do nome)       | Saber o nome do arquivo mas não onde ele está                |
| **Ctrl+Shift+F**      | Buscar texto em **todos** os arquivos do projeto         | Achar onde uma função é chamada, onde um texto aparece       |
| **Ctrl+Shift+P**      | Abrir a paleta de comandos (todos os comandos do VSCode) | "Format Document", "Reload Window", etc.                     |
| **F12**               | Ir até a **definição** da função/variável sob o cursor   | Está olhando uma chamada e quer ver o código que ela executa |
| **Alt+←** / **Alt+→** | Voltar / avançar no histórico de cursor                  | Depois de pular com F12, voltar de onde veio                 |

### Exemplo de uso real

Você está lendo `gameplay.c` e vê:

```c
projeteis_criar(jogo, jogo->jogador.pos, direcao, 1, ...);
```

Quer saber o que `projeteis_criar` faz?

1. **Clique no nome** `projeteis_criar`
2. **Aperte F12** → o VSCode pula para a definição em `projeteis.c`
3. Leia o código
4. **Aperte Alt+←** para voltar para onde você estava

## 3. Buscar coisas no projeto (Ctrl+Shift+F)

Esta é a ferramenta mais útil de todas. Abre uma busca global.

### Exemplos práticos

**Onde está definida a função `desenhar_menu`?**

- Aperte **Ctrl+Shift+F**
- Digite: `desenhar_menu`
- Os resultados vão mostrar tanto a declaração (em `.h`) quanto a definição (em `.c`)

**Onde a constante `GRAVIDADE` é usada?**

- Aperte **Ctrl+Shift+F**
- Digite: `GRAVIDADE`
- Vai mostrar a definição em `configuracao.h` e todos os usos

**Quais textos aparecem na tela do menu?**

- Aperte **Ctrl+Shift+F**
- Digite: `desenhar_texto_centralizado`
- Cada resultado é uma chamada que desenha texto

**Truques da busca:**

- Marque **`Aa`** (case-sensitive) se quiser diferenciar maiúsculas
- Marque **`abc`** (whole word) para evitar matches parciais
- Use **`.*`** com o ícone `.*` ativado para regex
- Filtre por tipo de arquivo no campo "files to include": `*.c` ou `src/lib/*.c`

## 4. Ver o que uma função recebe (hover)

**Passe o mouse** sobre o nome de qualquer função (sem clicar) e espere meio segundo. O VSCode mostra um tooltip com:

- A **assinatura** completa (tipo de retorno + parâmetros)
- O **arquivo** onde foi declarada
- Um trecho de código da declaração

Exemplo: passe o mouse sobre `renderizar_retangulo` em qualquer `.c` e você vê:

```text
void renderizar_retangulo(float x, float y, float largura, float altura, Cor cor)
```

Isso te diz instantaneamente que ela quer 4 floats e uma `Cor`, sem você
precisar abrir `renderizar.h`.

> **Pré-requisito**: instale a extensão **C/C++** da Microsoft (id:
> `ms-vscode.cpptools`) — sem ela o hover e o F12 não funcionam para C.

## 5. Navegar dentro de um arquivo

| Atalho           | O que faz                                                                   |
| **Ctrl+G**       | Pular para o número de linha (digite o número e Enter)                      |
| **Ctrl+Shift+O** | Listar todas as funções/símbolos do arquivo atual                           |
| **Ctrl+T**       | Listar todas as funções do **projeto inteiro** (símbolos do workspace)      |
| **Ctrl+F**       | Buscar texto **apenas no arquivo atual**                                    |
| **Ctrl+H**       | Substituir texto no arquivo atual                                           |
| **Ctrl+D**       | Selecionar a próxima ocorrência da palavra selecionada (para editar várias) |

### Truque: editar várias ocorrências de uma vez

1. Selecione a palavra (ex: o nome de uma variável)
2. Aperte **Ctrl+D** repetidamente — cada Ctrl+D adiciona o próximo match à
   seleção
3. Digite o novo nome — todos são alterados juntos

Útil para renomear variáveis dentro de uma única função sem mexer no resto.

## 6. Renomear símbolos com segurança (F2)

Para renomear uma função/variável em **todo o projeto** de forma consistente:

1. Coloque o cursor sobre o nome
2. Aperte **F2**
3. Digite o novo nome e Enter
4. O VSCode atualiza todas as referências (incluindo `.h`, `.c`, etc.)

**Isto é muito mais seguro do que Find & Replace**, porque o F2 entende
escopo: não renomeia uma variável local com o mesmo nome em outra função.

## 7. Compilar e rodar sem sair do VSCode

O terminal integrado fica em **Ctrl+`** (a tecla acima do Tab). Por padrão
abre PowerShell. Comandos úteis:

```powershell
# Compilar (script do projeto)
powershell -ExecutionPolicy Bypass -File .\scripts\compile_build.ps1

# Rodar
.\src\orbit_siege.exe

# Compilar via CMake (alternativa)
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

Se quiser **botão de play**, instale a extensão **CodeLLDB** ou use **Tasks**:
abra `.vscode/tasks.json` (crie se não existir) e defina uma task que rode
`compile_build.ps1`. Aí basta apertar **Ctrl+Shift+B** para compilar.

## 8. Achando algo específico no Orbit Siege

Tabela de "onde está X" — use Ctrl+Shift+F com a palavra-chave:

| O que você quer mudar             | Busque por                             | Arquivo provável             |
| Texto do menu inicial             | `ORBIT SIEGE` ou `desenhar_menu`       | `src/lib/desenhar.c`         |
| Cor de fundo / chão / lua         | `renderizar_fundo`                     | `src/lib/renderizar.c`       |
| Aparência do jogador              | `renderizar_jogador`                   | `src/lib/renderizar.c`       |
| Cor / forma dos inimigos          | `renderizar_inimigo`                   | `src/lib/renderizar.c`       |
| Velocidade do projétil do jogador | `JOGADOR_INICIAL_VELOCIDADE_PROJETIL`  | `src/include/configuracao.h` |
| Gravidade / pulo                  | `GRAVIDADE` ou `JOGADOR_FORCA_PULO`    | `src/include/configuracao.h` |
| Movimento do jogador (teclas)     | `gameplay_atualizar_jogador_movimento` | `src/lib/gameplay.c`         |
| Colisão entre formas              | `colisao_circulo_vs`                   | `src/lib/colisao.c`          |
| Tela de pausa                     | `desenhar_pausa`                       | `src/lib/desenhar.c`         |
| Áudio (música, tiro, pulo)        | `audio_gerar_`                         | `src/lib/audio.c`            |
| Upgrades (cartas e ícones)        | `desenhar_melhorias_tela`              | `src/lib/desenhar.c`         |
| Geração de ondas / inimigos       | `cenario_criar_onda`                   | `src/lib/cenario.c`          |
| Persistência (placar, settings)   | `persistencia_`                        | `src/lib/persistencia.c`     |

Quando estiver em dúvida sobre onde algo mora, **comece sempre por
Ctrl+Shift+F** com uma palavra do texto que aparece na tela ou o nome de uma
função relacionada.

## 9. Ler erros de compilação

Erros do GCC/MinGW aparecem no **terminal integrado** depois de rodar o
script de build. O formato é:

```text
src/lib/foo.c:123:45: error: 'bar' undeclared (first use in this function)
```

- `src/lib/foo.c` → caminho do arquivo
- `:123:45` → linha 123, coluna 45
- O texto depois de `error:` ou `warning:` → a mensagem

**Truque**: no terminal do VSCode, **Ctrl+click** sobre o caminho
`src/lib/foo.c:123:45` abre o arquivo direto na linha errada.

## 10. Coisas que dão dor de cabeça e como evitar

- **Mudou um `.h` e o build não pegou**: rode `compile_build.ps1` de novo,
  ou apague a pasta `build/` e recompile do zero. CMake às vezes não
  redetecta dependências de headers.
- **Função "não declarada"**: verifique se você incluiu o `.h` no topo do
  `.c` (`#include "nome.h"`). Lembre-se que `src/include/` já está no path.
- **`gcc` não é reconhecido**: o MinGW não está no PATH. Reinstale ou rode
  `where gcc` no terminal para confirmar.
- **`libfreeglut.dll` não encontrado**: rode `scripts/setup_libs.ps1` para
  baixar e compilar o FreeGLUT.
- **VSCode reclamando de include de OpenGL**: é só o IntelliSense errando,
  o build real funciona. Para silenciar, crie `.vscode/c_cpp_properties.json`
  apontando para os includes do FreeGLUT em `src/third_party/`.

## 11. Extensões recomendadas

Instale via **Ctrl+Shift+X** e busque o id entre parênteses:

- **C/C++** (`ms-vscode.cpptools`) — IntelliSense, hover, F12 — **obrigatório**
- **CMake Tools** (`ms-vscode.cmake-tools`) — integração CMake na sidebar
- **C/C++ Themes** (`ms-vscode.cpptools-themes`) — destaque de sintaxe
  melhorado para C
- **GitLens** (`eamodio.gitlens`) — ver quem editou cada linha
- **Better Comments** (`aaron-bond.better-comments`) — colore TODO/FIXME

## 12. Conclusão

Se você lembrar de **apenas três coisas** deste guia:

1. **Ctrl+Shift+F** para achar qualquer coisa no projeto
2. **F12** para pular para a definição da função sob o cursor
3. **Hover do mouse** para ver os argumentos de uma função sem abrir o `.h`

…você já navega o Orbit Siege confortavelmente.

Bom desenvolvimento.
