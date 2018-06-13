# FEATURES

#### MULTITHREADING:
O servidor consistirá em n threads para realizar o controle das conexões, mensagens e chamadas de funções executadas no servidor. Um conjunto de n encadeamentos estará sempre pronto para executar/atender solicitações de entrada.

#### QUEUING
O encadeamento das filas estará escutando continuamente a porta "p" para solicitações de entrada. Assim que uma nova solicitação chegar ao servidor master_server, ela será inserida na fila pronta.

#### SCHEDULING
A política de agendamento a ser usada é definida através da opção [–s sched] quando o servidor master_slave for iniciado pela primeira vez. As políticas disponíveis são First Come First Serve (FCFS) e Shortest Job First (SJF).

#### SYCHNRONIZATION
Para garantir que protegemos a fila pronta e outras estruturas de dados compartilhadas em vários encadeamentos para evitar condições de corrida. Os bloqueios de mutex foram implementados para o mesmo.

#### NOBLOCKING
O modo padrão para programas de socket é Blocking, porém utilizamos o metodo Nonblocking. Isso significa que para chamadas FCNTL () ou IOCTL (), o programa de chamada continuará, mesmo que a chamada de E/S possa não ter sido concluída. Se a chamada de E/S não puder ser concluída, ela retornará com ERRNO EWOULDBLOCK. Para testarmos a conclusão de qualquer chamada de soquete, utlizamos a função SELECT() que retorna EWOULDBLOCK.

## COMPILE AND RUN

#### MASTER_SERVER

    Command:

    > gcc -c master_server.c functions.c actions.c color.h && gcc master_server.c functions.c actions.c color.h -std=c99 -lpthread -Wall -o master_server && ./master_server [port]

    Example:

    > gcc -c master_server.c functions.c actions.c color.h && gcc master_server.c functions.c actions.c color.h -std=c99 -lpthread -Wall -o master_server && ./master_server 8000

#### CLIENT

    Command:
    gcc -c client.c color.h && gcc client.c color.h -std=c99 -Wall -o client && ./client [hostname] [server_ip] [server_port]
    
    Example:    
    gcc -c client.c color.h && gcc client.c color.h -std=c99 -Wall -o client && ./client $('hostname') 192.168.10.2 8000

# Documentação

## Client

- void chatloop(char *hostname, int socketFd);
- void buildMessage(char *result, char *hostname, char *msg);
- void setupAndConnect(struct sockaddr_in *serverAddr, struct hostent *host, int socketFd, long port);
- void setNonBlock(int fd);
- void interruptHandler(int sig);

### setupAndConnect()

**Declaração:**
- void setupAndConnect(struct [address serveraddr], struct [hostname], int [socket descriptor], long [port]);

**Definição:**
- Utilizamos para realizar o setup da conexão.

**Funcionamento:**
1. Utilizamos memset para preencher a memória com valores 0.
2. Configuramos as variáveis de conexão para serverAddr.
3. O cliente tenta realizar uma conexão com o servidor, sendo que um retorno da função connect() < 0 significa que houve um erro ao tentar conectar.

### chatLoop()

**Declaração:**
- void chatloop(char *hostname, int socketFd);

**Definição:**
- Utilizamos para receber a entrada do cliente e mostrar a saída.

**Funcionamento:**
1. Instanciamos um struct que recebe um fd (clientFds) válido, e três arrays do tipo char para o buffer.
2. O array fullMsg contém todas as informações geradas pela função buildMessage.
3. O array chatBuffer recebe a entrada do cliente, já o msgBuffer é para receber a resposta do servidor.
4. Iniciamos o loop e redefinimos o conjunto de fd cada vez que select () o modifica.
5. Esperamos por um fd válido utilizando a função select() != -1
6. 

### setNonBlock()

**Declaração:**
- void setNonBlock(int [file_descriptor]);

**Definição:**
- Utilizado para definirmos as chamadas sockets para NONBLOCK

**Funcionamento:**
1. Utilizamos a função fcntl(fd, F_GETFL) com o parâmetro F_GETFL que retorna as flags do fd correspondente.
2. Atribuimos o retorno a variável 'flags' e verificamos se há alguma flag atribuída.
3. Se haver alguma flag atribuída atribuimos a flag o modo nonblock pelo parâmetro O_NONBLOCK
4. Concluímos utilizando a função fcntl(fd, F_SETFL, flags) para definir a flag do fd

### interruptHandler()

**Declaração:**
- void interruptHandler(int sig_unused)

**Definição:**
- Utilizado para notificar o servidor quando o cliente executa o comando /exit

**Funcionamento:**
1. No bloco de execução da função chatLoop, é identificado o comando /exit
2. A função interruptHandler é chamada com o parâmetro -1
3. Utilizamos a função close() passando o file_descriptor do socket do cliente
4. A função exit(1) termina o programa

## Server

- void startConn(int socketFd);
- void bindSocket(struct sockaddr_in *serverAddr, int socketFd, long port);
- void removeClient(connDataVars *data, int clientSocketFd);
- void *newClientHandler(void *data);
- void *clientHandler(void *chv);
- void *messageHandler(void *data);
- void queueDestroy(queue *q);
- queue* queueInit(void);
- void queuePush(queue *q, char* msg);
- char* queuePop(queue *q);
- void buildMessage(char *fullMsg, char *msgBuffer, int clientSocketFd);
- int splitBuffer(char *fullMsg);

### startConn()

**Declaração:**
- void startConn(int socketFd);

**Definição:**
- Quando o cliente solicita uma conexão, esta função à configura e cria uma threads para controlar a conexão e outra para a troca de menssagens.

**Funcionamento:**
1. Definimos as variáveis de conexão.
2. Para data.clientListMutex, alocamos um espaço da memória com malloc() e definimos o tipo pthread_mutex_t.
3. Logo, iniciamos o mutex com pthread_mutex_init(data.clientListMutex, NULL), o valor NULL utilizado significa que utilizaremos os atributos padrão do mutex conforme descrito na documentação. [Doc] (http://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_mutex_init.html)
4. 


chatBuffer[] = mensagem que o cliente digitou
msgBuffer[] = recebe uma mensagem do servidor
fullMsg[] = buffer contendo todas as informações
