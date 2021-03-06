# FEATURES

#### MULTITHREADING:
O servidor consistirá em n threads para realizar o controle das conexões, mensagens e chamadas de funções executadas no servidor. Um conjunto de n encadeamentos estará sempre pronto para executar/atender solicitações de entrada.

#### QUEUING
O encadeamento das filas estará escutando continuamente a porta "p" para solicitações de entrada. Assim que uma nova solicitação chegar ao servidor master_server, ela será inserida na fila.

#### SCHEDULING
As políticas disponíveis são First Come First Serve (FCFS) e Shortest Job First (SJF).

#### SYCHNRONIZATION
Para garantir e proteger a fila e outras estruturas de dados compartilhadas em vários encadeamentos para evitar condições de corrida, implementamos bloqueios por mutex.

#### NOBLOCKING
O modo padrão para programas de socket é Blocking, porém utilizamos o metodo Nonblocking. Isso significa que para chamadas FCNTL() ou IOCTL(), o programa de chamada continuará, mesmo que a chamada de E/S possa não ter sido concluída. Se a chamada de E/S não puder ser concluída, ela retornará com ERRNO EWOULDBLOCK. Para testarmos a conclusão de qualquer chamada de soquete, utlizamos a função SELECT() que retorna EWOULDBLOCK.

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
2. data.queue recebe o ptr da função queueInit() na qual tem por função iniciar a fila e alocar espaço para ela.
3. Para data.clientListMutex, alocamos um espaço da memória com malloc() e definimos o tipo pthread_mutex_t.
4. Logo, iniciamos o mutex com pthread_mutex_init(data.clientListMutex, NULL), o valor NULL utilizado significa que utilizaremos os atributos padrão do mutex.
5. Criamos uma thread para controlar a conexão do cliente e passamos data* como parâmetro que contém as informações da conexão.
3. Com FD_ZERO() definimos '0' nos bits do file_descriptor.
7. Com FD_SET() definimos o file_descriptor.
8. Criamos uma thread para controlar a troca de mensagens entre os hosts.
9. Com pthread_join aguardamos até a thread de destino seja encerrada.
10. Limpamos a fila com queueDestroy().
11. Utilizando pthread_mutex_destroy(data.clientListMutex) destruimos o objeto mutex.
12. Após destruirmos o mutex, desalocamos o espaço de memória utilizado pela função destruída.

### queueInit()

**Declaração:**
- queue* queueInit(void);

**Definição:**
- Esta função inicia e aloca espaço para a fila de mensagens.

**Funcionamento:**
1. Alocamos espaço na memória com malloc().
2. Inicializamos as variáveis da struct.
3. Alocamos espaço na memória com malloc() para q->mutex, q->notFull e q->notEmpty, Logo, iniciamos as variáveis de condição utilizando pthread_cond_init().
4. Retornamos o ponteiro da fila que é utilizado por startConn, pela variável data.queue.

### queueDestroy()

**Declaração:**
- void queueDestroy(queue *q);

**Definição:**
- Esta função limpa a fila.

**Funcionamento:**
1. Utilizamos pthread_mutex_destroy() em q->mutex, q->notFull e q->notEmpty para limpar o conteúdo da fila.
2. Ainda utilizamos free() para desalocar espaço na memória para q->mutex, q->notFull e q->notEmpty, inclusive para a fila representada pelo ponteiro *q.

### queuePush()

**Declaração:**
- void queuePush(queue *q, char* msg);

**Definição:**
- Empurra a mensagem para o final da fila.

**Funcionamento:**
1. Com q->buffer[q->tail], acessamos o final da fila e empurramos a menssagem para este espaço.
2. q->tail que é a variável que representa o valor do final da fila é incrementada.
3. Verificamos se q->tail == MAX_BUFFER, isso significa que o espaço alocado para a fila é igual ao buffer e a fila está cheia.
4. Verificamos se q->tail == q->head, se for significa que a fila está cheia.
5. q->empty recebe o valor 0, que significa que algo foi alocado na fila.

### queuePop()

**Declaração:**
- char* queuePop(queue *q);

**Definição:**
- Tiramos a mensagem do topo da fila.

**Funcionamento:**
1. Com q->buffer[q->tail], acessamos o final da fila e empurramos a menssagem para este espaço.
2. q->head que é a variável que representa o valor do final da fila é incrementada.
3. Verificamos se q->head == MAX_BUFFER, isso significa que a fila já foi toda percorrida, então q->head recebe 0 e volta para o início da fila.
4. Verificamos se q->head == q->head, se for significa que a fila está vazia, então q->empty recebe valor 0.
5. q->full recebe o valor 0, que significa que algo foi retirado da fila e ela não está mais cheia.

### bindSocket()

**Declaração:**
- void bindSocket(struct sockaddr_in *serverAddr, int socketFd, long port);

**Definição:**
- Associamos o socket do servidor com seu endereço local para que o servidor se 'ligue', para que os clientes possam usar esse endereço para se conectar ao servidor.

**Funcionamento:**
1. Utilizando memset zeramos o endereço de serverAddr.
2. Configuramos AF_INET para podermos realizar conexões pela internet.
3. A porta padrão do servidor está definida para 8000, mas ao inicializarmos o servidor podemos passar por parâmetro outra porta.

### removeClient()

**Declaração:**
- void removeClient(connDataVars *data, int clientSocketFd);

**Definição:**
- Remove o soquete da lista de soquetes ativos do cliente e o fecha.

**Funcionamento:**
1. Utilizando pthread_mutex_lock(data->clientListMutex) na lista de clientes para manipularmos esta fila.
2. Percorremos a lista para achar o socket do cliente que será encerrado.
3. Utilizamos a função close() para fechar a conexão.
4. Decrementamos data->numClients, que representa o número de clientes conectados.

### newClientHandler()

**Declaração:**
- void *newClientHandler(void *data);

**Definição:**
- Thread para lidar com novas conexões. Adiciona o fd do cliente à lista de fds de clientes e gera uma nova thread clientHandler para ele.

**Funcionamento:**
1. Criamos um loop infinito para esta função utilizando while(1).
2. clientSocketFd recebe o retorno do fd do cliente da função accept() que é utilizada para criar um socket para o cliente que solicita um connect().
3. Bloqueamos a lista de clientes para adicionar um novo cliente, e adiciona este cliente a lista se o número máximo de clientes não for excedido.
4. Utilizando FD_ISSET negado, esperamos que ele retorne 1 se não haver nenhum file_descriptor definido no conjunto de file_descriptor apontado por fd_set conforme função FD_ISSET(fd , &fdset).
5. Incluimos clientSocketFd ao conjunto de file_descriptor apontado por &(chatData->serverReadFds).
6. Criamos uma nova thread para lidar com as mensagens do cliente, definida na função clientHandler().
7. Se a thread for criada incrementamos (chatData->numClients) o número de clientes, senão fechamos o socked utilizando close().
8. Por fim liberamos o mutex da lista de clientes com pthread_mutex_unlock().

### clientHandler()

**Declaração:**
- void *clientHandler(void *chv);

**Definição:**
- O "produtor" - ouve as mensagens do cliente para adicionar à fila de mensagens.

**Funcionamento:**
1. Definimos as variáveis de buffer msgBuffer e fullMsg.
2. msgBuffer[] é o buffer no qual conteúdo é uma mensagem recebida do servidor.
3. fullMsg[] é o buffer no qual contem todas as informações [clientSocketFd:hostname:message].
4. Criamos um loop infinito para a função.
5. Se o cliente digitar /exit removemos ele da lista de clientes e fechamos o socket.
6. Senão aguardamos a fila não estar cheia antes de enviar mensagem, e comparamos com pthread_cond_wait() para verificar a situação do mutex aplicado à fila.
7. Obtemos o bloqueio da fila com pthread_mutex_lock().
8. Construimos o buffer fullMsg chamando buildMessage(fullMsg, msgBuffer, clientSocketFd).
9. Enviamos a mensagem para a fila chamando queuePush(q, fullMsg).
10. Desbloqueamos a fila utilizando pthread_mutex_unlock(), e definimos a variável de condição com pthread_cond_signal(q->notEmpty).
11. Essa função é usadas para desbloquear encadeamentos bloqueados em uma variável de condição.

### messageHandler()

**Declaração:**
- void *messageHandler(void *data);

**Definição:**
- O "consumidor" - espera que a fila tenha mensagens, então as retira e transmite para os clientes.

**Funcionamento:**
1. Definimos as variáveis para manipular.
2. Iniciamos um loop infinito para a função.
3. Obtemos o bloqueio da utilizando filapthread_mutex_lock().
4. Aguardamos ela não estar vazia utilizando pthread_cond_wait(q->notEmpty, q->mutex).
5. Chamamos queuePop(q) para retirar a mensagem da fila.
6. Desbloqueamos a fila utilizando pthread_mutex_unlock().
7. Definimos a variável de condição utilizando pthread_cond_signal(q->notFull).
8. Essa função é usadas para desbloquear encadeamentos bloqueados em uma variável de condição.
9. Preparamos a mensagem para enviar para o cliente utilizando a função splitBuffer(fullMsg), no qual separa as informações contidas no buffer utilizando delimitadores com a função strsep().
10. Após isso enviamos a mensagem chamando a função write().
