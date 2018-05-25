# Trabalho-SO
Trabalho destinado a disciplina de Arquitetura de Sistemas operacionais - UFSC

Enunciado do trabalho
Desenvolver um sistema de arquivos distribuídos baseado no padrão EXT3 da família de Sistemas Operacionais Linux. O sistema deve permitir que arquivos locais sejam acessados por usuários remotos simultaneamente. As operações permitidas pelo sistema são: criar arquivo ou diretório, remover arquivo ou diretório, copiar dados de um diretório para outro e listar conteúdo de diretórios.

Etapa 1

Desenvolver a estrutura de acesso do servidor de arquivos. Ele será acessado via socket TCP. Cada conexão será gerida por uma thread. Não é necessário autenticação dos usuários. O acesso concorrente ao mesmo arquivo deve ser garantido com o uso de semáforos/mutexes. Simule o sistema de arquivos utilizando o sistema de arquivos do sistema operacional que você está usando. Para demonstrar o funcionamento, não é necessário criar um aplicativo cliente. Você pode usar o aplicativo netcat disponível para Linux e Windows.

Recomenda-se que o servidor imprima mensagens na tela para demonstrar o funcionamento ao professor. 3.

Etapa 2

Estenda o trabalho desenvolvendo a estrutura de alocação e organização de arquivos do servidor de arquivos. O servidor de arquivos deve seguir as mesmas diretrizes do sistema de arquivos EXT3. O disco do servidor deve ser simulado através de um único arquivo no sistema de arquivos do sistemas operacional onde o servidor está sendo executado. A thread no servidor deve receber uma solicitação do cliente e então repassá-la a um novo processo via pipe nomeado e este deverá executar a ação solicitada e devolver ao servidor uma mensagem de status, informando se a solicitação foi realizada com sucesso ou com erro.

Informações

O trabalho deverá ser feito em dupla ou individualmente. No dia da apresentação, data que deverá ser marcada com antecedência com o professor, o aluno ou alunos irão apresentar o programa em execução mostrando as funcionalidades do mesmo, bem como apresentar o código fonte. As notas serão atribuídas de maneira individual, ou seja, caso o trabalho tenha sido feito em dupla cada um dos autores receberá uma nota. Esta nota dependerá do desempenho durante apresentação do trabalho onde o professor arguirá o aluno. O trabalho deverá ser desenvolvido na linguagem de programação C usando as bibliotecas de semáforos, threads e comunicação entre processos (socket local ou pipe nomeado).

# FEATURES

## MASTER SERVER

### MULTITHREADING:
O servidor consistirá em n threads para realizar o controle das conexões, mensagens e chamadas de funções executadas no servidor. Um conjunto de n encadeamentos estará sempre pronto para executar/atender solicitações de entrada.

### QUEUING
O encadeamento das filas estará escutando continuamente a porta "p" para solicitações de entrada. Assim que uma nova solicitação chegar ao servidor master_server, ela será inserida na fila pronta.

### SCHEDULING
A política de agendamento a ser usada é definida através da opção [–s sched] quando o servidor master_slave for iniciado pela primeira vez. As políticas disponíveis são First Come First Serve (FCFS) e Shortest Job First (SJF).

### SYCHNRONIZATION
Para garantir que protegemos a fila pronta e outras estruturas de dados compartilhadas em vários encadeamentos para evitar condições de corrida. Os bloqueios de mutex foram implementados para o mesmo.

# COMPILE AND RUN

### Server

    Command:

    gcc -c master_server.c functions.c actions.c color.h && gcc master_server.c functions.c actions.c color.h -std=c99 -lpthread -Wall -o master_server && ./master_server [port]

    Example:
    
    gcc -c master_server.c functions.c actions.c color.h && gcc master_server.c functions.c actions.c color.h -std=c99 -lpthread -Wall -o master_server && ./master_server 8000

### Client

    Command:

    gcc -c client.c color.h && gcc client.c color.h -std=c99 -Wall -o client && ./client [hostname] [server_ip] [server_port]
    
    Example:
    
    gcc -c client.c color.h && gcc client.c color.h -std=c99 -Wall -o client && ./client $('hostname') 192.168.10.2 8000