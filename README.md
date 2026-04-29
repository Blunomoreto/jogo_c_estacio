# Orbit Siege (C + OpenGL/GLUT)

Jogo 2D platformer com ação em C estruturado (sem OO), onde você controla um humano em um cenário extraterrestre com gravidade e mecânicas de pulo, survivalidade com melhorias.

## Estrutura do repositório

O repositório não deve conter conteúdos de terceiros, estes serão baixados e instalados a partir de scripts de setup.

A pasta scripts contém os scripts de instalação e compilação utilizados para construir o projeto.

A pasta `src` contém os arquivos críticos de execução.

A raíz do repositório contém apenas arquivos instrucionais e de setup.

## Requisitos atendidos

- Linguagem: C
- Bibliotecas mínimas: OpenGL + GLUT
- Figuras: círculo, retângulo, triângulo, losango, estrela
- Efeitos: brilho aditivo, transparência (alpha blending), parallax/partículas
- Mecânicas: tempo, pontuação, colisão, animação, **gravidade e pulo**
- Interação: teclado + mouse
- Estrutura de jogo: menu inicial, pausa, vitória/derrota
- Extras: áudio simples, imagens (PPM), persistência de score e wave, screenshot nativa
- Progressão: upgrades únicos por rodada + reroll com ouro
- Menu: exibe top scores salvos localmente
- Fase final: mini-boss com barra de vida e padrão de tiro em leque
- Variedade de inimigos: padrão, sniper e tank com comportamentos distintos
- Menu de opções: áudio on/off e dificuldade (fácil/normal/difícil), persistidos em arquivo
- Tela de placar completa com paginação e opção de limpar histórico
- Polimento de upgrade: ícones visuais, tooltip por hover e feedback de seleção
- Feedback de interface: notificações rápidas na HUD (screenshot, settings, reroll, save)
- UI refinada: botões com hover animado, brilho pulsante e paletas visuais por contexto
- Layout responsivo: escalonamento de painéis/botões para diferentes resoluções
- Modo compacto: redução automática de textos auxiliares em janelas menores
- **Personagem humanoide** com renderização de cabeça, corpo e membros
- **Cenário com chão e lua de fundo** para imersão visual

## Controles

- `A` / `D` ou setas ← →: mover horizontalmente
- `SPACE` ou `W`: pular (gravidade automática, máx 2x altura do player)
- Mouse: mirar
- Botão esquerdo: atirar
- `P` ou `ESC`: pausar/retomar
- `F12`: salvar screenshot (`screenshots/*.ppm`)
- Menu: `ENTER`/`SPACE` ou clique em START
- Menu: `O` ou botão OPTIONS abre configurações
- Menu: `L` ou botão SCOREBOARD abre ranking completo
- Upgrade: teclas `1` `2` `3` ou clique
- Upgrade reroll: `R` (custo 3 de ouro)
- Ranking: `A`/`D` muda página, `C` limpa histórico, `M` volta ao menu

## Build (CMake)

```bash
cmake -S . -B build
cmake --build build
```

No Windows, instale FreeGLUT e garanta `freeglut` disponível para link.

Guia completo de instalação: [INSTALL.md](INSTALL.md)

## Troubleshooting rápido

- Se não compilar por falta de `cmake`, instale CMake e reabra o terminal.
- Se faltar `freeglut`, instale a biblioteca e confirme o linker do ambiente.
- Se não houver áudio, valide presença dos arquivos WAV em `assets/audio`.
- Se o fundo não carregar, mantenha um `background.ppm` válido em `assets/images`.

## Assets opcionais

- `assets/images/background.ppm` (P6)
- `assets/audio/bgm.wav`
- `assets/audio/shoot.wav`
- `assets/audio/hit.wav`

Sem assets, o jogo continua funcional com fallback visual/silencioso.

## Checklist de apresentação (demo)

- Abrir menu e mostrar `START`, `OPTIONS`, `SCOREBOARD`
- Em `OPTIONS`, alternar áudio e dificuldade (confirmar persistência)
- Iniciar run, mostrar controles teclado + mouse
- Demonstrar colisão, dano, pontuação, tempo e upgrades
- Usar `F12` e confirmar notificação de screenshot
- Completar waves até mini-boss final
- Em game over, salvar nome/pontuação e abrir ranking paginado

## Matriz de conformidade (JSON de requisitos)

| Requisito         | Status | Evidência no jogo                                           |
| Linguagem C       | ✅       | Projeto completo em C estruturado                           |
| OpenGL + GLUT     | ✅       | Renderização, input e loop via GLUT/OpenGL                  |
| Mínimo 5 figuras  | ✅       | Círculo, retângulo, triângulo, losango, estrela             |
| Mínimo 3 efeitos  | ✅       | Brilho aditivo, transparência, parallax/partículas          |
| Tempo             | ✅       | `timeLeft` no HUD e derrota por tempo                       |
| Pontuação         | ✅       | `score` por sobrevivência e eliminações                     |
| Colisão           | ✅       | Círculo-círculo e círculo-AABB (inimigos/projéteis/cenário) |
| Animação          | ✅       | Pulso do player, órbita inimiga, partículas                 |
| Teclado + mouse   | ✅       | Movimento, menu, opções, tiro, seleção                      |
| Menu inicial      | ✅       | Tela inicial com Start/Options/Scoreboard                   |
| Pausa             | ✅       | `P` ou `ESC`                                                |
| Vitória/derrota   | ✅       | Telas finais com fluxo de save                              |
| Áudio             | ✅       | BGM + SFX com toggle de áudio                               |
| Imagens           | ✅       | Textura de fundo PPM opcional                               |
| Persistência      | ✅       | `stats.dat`, `scoreboard.dat`, `settings.dat`               |
| Screenshot nativa | ✅       | `F12` com `glReadPixels` para `screenshots/*.ppm`           |
| Histórico local   | ✅       | Ranking completo paginado + limpar histórico                |

## Presets de balanceamento (dificuldade)

| Preset | HP inimigo | Dano inimigo | Cadência inimiga    | Tempo inicial | Waves p/ vitória | Multiplicador de score |
| EASY   | 0.86x      | 0.82x        | 1.16x (mais lenta)  | +40s          | -1               | 0.95x                  |
| NORMAL | 1.00x      | 1.00x        | 1.00x               | base          | base             | 1.00x                  |
| HARD   | 1.25x      | 1.24x        | 0.84x (mais rápida) | -25s          | +1               | 1.15x                  |

## Roteiro de demonstração (5 minutos)

1. **00:00–00:40** — Menu inicial
Mostrar `START`, `OPTIONS`, `SCOREBOARD`.

2. **00:40–01:20** — Configuração
Entrar em `OPTIONS` e alternar áudio + dificuldade.
Voltar ao menu e destacar persistência das configurações.

3. **01:20–03:20** — Gameplay principal
Movimentar com teclado e mirar/atirar com mouse.
Mostrar colisões (inimigo/projétil/cenário), HUD, pontuação e tempo.
Abrir tela de upgrades, selecionar upgrade e usar reroll.

4. **03:20–04:20** — Efeitos e extras
Destacar brilho, transparência, partículas e animações.
Tirar screenshot com `F12` e mostrar notificação.

5. **04:20–05:00** — Encerramento de run
Mostrar vitória/derrota, salvar nome/pontuação.
Abrir ranking completo com paginação.

## Notas de UI/UX (mar/2026)

- Menu principal, opções, placar, pausa e telas finais com estilo visual consistente.
- Hover com transição suave (interpolação por frame) e realce de foco nos botões.
- Tela de upgrades com cards responsivos, tooltip alinhado e reroll destacado.
- Hitboxes de clique alinhadas ao layout responsivo para manter precisão de interação.
