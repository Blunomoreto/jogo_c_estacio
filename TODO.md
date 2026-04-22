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

### Keybinds no menu

Atualmente os itens do menu possuem keybinds para ativação (comportamento esperado?)

## Back

### Suporte a outras plataformas

Outras plataformas como linux ou mac não utilizam powershell, logo o script .ps1 não funcionará como esperado, é necessário criar scripts de instalação para estes sistemas. (OBS: NÃO É NECESSÁRIO, PODE SER LISTADO COMO FEITO APENAS PARA WINDOWS)

### Criação de arquivos internos automáticamente

Arquivos dentro da pasta `data` devem ser criado automáticamente durante a inicialização se não existirem.

### Melhoria nos projéteis guiados

Diminuição da capacidade de manobra e aumento da velocidade para induzir maiores erros de guiança

### Duração de partida

Aumentar a quantidade de `rounds` para 10 com uma progressão de dificuldade mais suave.

### Upgrade de velocidade

Criar um novo upgrade para aumentar a velocidade do projétil não guiado (projétil guiado deve manter velocidade fixa)

### Separação

Separar seções com funções do projeto em arquivos dentro da pasta lib
