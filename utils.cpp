#include "header.h"

// Un regalito, puede que quieran modificarla
// Dado un arreglo de char str y un socket descriptor s, hace una lectura
// bloqueante sobre s y llena el buffer str con los bytes leídos.
int read_sock(char str[], int s) 
{
    int n;
    n = recv(s, str, 2*MENSAJE_MAXIMO, 0);

    if (n == 0) 
        return -1;
    if (n < 0){ 
        perror("recibiendo");
        exit(1);
    }
    
    str[n] = '\0';
    printf("%d\n",n);
    printf("recibi: %s\n",str);
    return 0;
}


// Dado un puntero a un request req y un socket s, recibe una request en s y la 
// almacena en req. La funcion es bloqueante
void get_request(struct request* request, int s)
{
    char req[MENSAJE_MAXIMO + 10];
    int n = recv(s, req, MENSAJE_MAXIMO + 10, 0);
    if (n < 0) { 
    	perror("ERROR: recibiendo");
    }
    strncpy(request->type,((struct request*)req)->type, 10);
    strncpy(request->msg, ((struct request*)req)->msg, MENSAJE_MAXIMO); 
}

void send_request(struct request* request, int socket)
{
	int s = send(socket, (char *) request , MENSAJE_MAXIMO + 10, 0);
	if (s < 0) { 
		perror("ERROR: enviando");
	}
}

// Dado un vector de enteros que representan socket descriptors y un request,
// envía a traves de todos los sockets la request.
void broadcast(vector<vector<int>> &sockets, struct request* request)
{
    for (size_t i = 0; i < sockets.size(); i++)
	{
		for (size_t j = 0; j < sockets.size(); j++)
		{
			send_request(request, sockets[i][j]);
		}
	}  
}

// Por siempre, acepta conexiones sobre un socket s en estado listen y 
// agrega los sockets asociados al vector v.
void accept_conns(int s, vector<int>& v, sem_t& semaforo)
{
    struct sockaddr_in remote;
	int t = sizeof(remote);
	int socket;
    if ((socket = accept(s, (struct sockaddr *)&remote, (socklen_t *)&t)) == -1)
    {
        perror("ERROR: aceptando cliente");
        exit(1);
    }
    cout << "New socket" << endl;
    cout << socket << endl;
    v.push_back(socket);
    sem_post(&semaforo);  
}

// Dado un puerto lsn_port devuelve un socket en estado listen asociado
// a todas las interfaces de red local y a ese puerto (ej 127.0.0.1:lsn_port)
int set_acc_socket(int lsn_port, sockaddr_in& local)
{
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("ERROR: socket server");
        exit(1);
    }

    local.sin_family = AF_INET;
    local.sin_port = htons(lsn_port);
    local.sin_addr.s_addr = INADDR_ANY;

    int localLink = bind(s, (struct sockaddr *)&local, sizeof(local));
    if (localLink < 0) {
        perror("ERROR: bind server");
        exit(1);
    }

    int listenMode = listen(s, 10);
    if (listenMode == -1) {
        perror("ERROR: listen server");
        exit(1);
    }

    return s;
}

// 
int conectarSocket(int port)
{
    struct sockaddr_in remote;
    
    int conectarSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (conectarSocket  == -1)
	{
		perror("ERROR: creación de socket server");
		exit(1);
	}

	remote.sin_family = AF_INET;
	remote.sin_port = htons(PORT);
	remote.sin_addr.s_addr = INADDR_ANY;

	int s = connect(conectarSocket, (struct sockaddr *)&remote, sizeof(remote));
	if (s == -1)
	{
		perror("ERROR: conexión a server");
		exit(1);
	}
    return conectarSocket;
}
