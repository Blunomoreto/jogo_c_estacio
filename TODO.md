# Lista de tarefas

## Front

### Correção do texto

O texto no menu e durante uma partida deve ser centralizado e contido nas caixas.

### Controle de resolução

- Limitar as resoluções através das opções no menu.
- Limitar resolução mínima para evitar que o menu quebre (sugestão: 640x480)

Como o jogo utiliza um sistema de coordenadas isso evita problemas na construção dos cenários, posições, tempo de vida dos projéteis etc.

### Plano de fundo

O plano de fundo muda conforme a resolução da janela é alterada (comportamento esperado?)
O arquivo não é salvo na pasta images durante inicialização (comportamento esperado?)

### Keybinds no menu

Atualmente os itens do menu possuem keybinds para ativação (comportamento esperado?)

## Back

### Balanceamento

O jogo deve ser balanceado, atualmente existem muitos inimigos com uma progressão imediata, aumente a quantidade de fases para 10 e diminua a quantidade de inimigos pro fase

### Melhoria nos projéteis guiados

Diminuir a quantidade de projéteis disparados por rajada, aumentar dano dos projéteis guiados do jogador

### Duração de partida

Aumentar a quantidade de `rounds` para 10 com uma progressão de dificuldade mais suave.

### Upgrade de velocidade

Criar um novo upgrade para aumentar a velocidade do projétil não guiado (projétil guiado deve manter velocidade fixa)

### Renomeação

Funções devem ser renomeadas para utilizar lingua portuguesa
Variáveis devem explicar a função delas e utilizar português (hp -> vida, score -> pontuacao etc.)

### Utilizar e reutilizar funções

Devem ser utilizadas e reutilizadas as novas funções clamp criadas e também outras funções aplicáveis
