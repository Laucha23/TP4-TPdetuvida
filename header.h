#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <thread>
#include <stdio.h>
#include <iostream>
#include <sys/sendfile.h>
#include <vector>
#include <semaphore.h>
#include <mutex>
#include <sys/wait.h>
#include <cassert>
#define PORT 5030
#define MENSAJE_MAXIMO  256
#define MAX_CLIENTS 10
#define CLIENT_READY "CL_OK\n"
#define CANT_CLIENTS 9

using namespace std;

// Ejemplo sencillo. 
// Recordar que es posible definir clases
// También se pueden definir funciones dentro del struct

struct request{
	char type[10];
	char msg[MENSAJE_MAXIMO];
};

// OPCIONAL
struct client_request{
	// TO DO
};

// Funciones utiles sugeridas
int read_sock(char str[], int s);
void broadcast(vector<int>& sockets, struct request* req);
void get_request(struct request* req, int s);
void send_request(struct request* req, int s)
int set_acc_socket(int lsn_port);
void accept_conns(int s, vector<int>& v );

