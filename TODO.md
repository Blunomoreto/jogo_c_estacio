# Lista de tarefas

## Estrutura do repositório

### Separação de bibliotecas do repositório

O repositório não pode conter bibliotecas ou aplicações de terceiros, a pasta `scripts` deve portar os scripts de instalação.

### Suporte a outras plataformas

Outras plataformas como linux ou mac não utilizam powershell, logo o script .ps1 não funcionará como esperado, é necessário criar scripts de instalação para estes sistemas.

### Criação de arquivos internos automáticamente

Arquivos dentro da pasta `data` devem ser criado automáticamente durante a inicialização se não existirem.

### Essencia de execução

Apenas arquivos críticos para a execução do jogo devem ser contidos na pasta src.
