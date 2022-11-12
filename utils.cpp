#include "header.h"

// Un regalito, puede que quieran modificarla
// Dado un arreglo de char str y un socket descriptor s, hace una lectura
// bloqueante sobre s y llena el buffer str con los bytes leídos.
int read_sock(int s) 
{
    int n;
	char str[MENSAJE_MAXIMO];
    n = recv(s, str, MENSAJE_MAXIMO, 0);

    if (n == 0) 
        return -1;
    if (n < 0) { 
        perror("Error read_sock");
        exit(1);
    }
    str[n] = '\0';
    printf("%d\n",n);
    printf("recibi: %s\n",str);
    return 0;
}

// Dado un puntero a un request req y un socket s, recibe una request en s y la 
// almacena en req. La funcion es bloqueante
void get_request(struct request* req, int s)
{
   char request[MENSAJE_MAXIMO + 10];
    int n = recv(s, request, MENSAJE_MAXIMO + 10, 0);
    if (n < 0) { 
    	perror("Error recibiendo");
        exit(1);
    } else if (n == 0) {
        perror("Se ha perdido la conexion con un socket");
        exit(1);
    }
    strncpy(req->type,((struct request*)request)->type, 10);
    strncpy(req->msg, ((struct request*)request)->msg, MENSAJE_MAXIMO); 
}

void send_request(int socket, struct request* req)
{
	int s = send(socket, (char *) req , MENSAJE_MAXIMO + 10, 0);
	if (s < 0) { 
		perror("Error enviando");
        exit(1);
	}
	
}

// Dado un vector de enteros que representan socket descriptors y un request,
// envía a traves de todos los sockets la request.
void broadcast(vector<vector<int>> &sockets, struct request* req)
{
	for (size_t i = 0; i < sockets.size(); i++)
	{
		for (size_t j = 0; j < sockets[i].size(); j++)
		{
            cout << to_string(sockets[i][j]) << endl;
			send_request(sockets[i][j], req);
		}
		
	}
}

void connection_handler(int socket_desc){
    
    while(1){
        if(read_sock(socket_desc) == -1)
            break;
    }
        
}
