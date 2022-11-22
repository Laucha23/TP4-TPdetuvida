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
#include <iostream>
#include <sstream>
#include<string>

#define VER  3
#define HOR  3
#define PORT 5032
#define MENSAJE_MAXIMO 256

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
void broadcast(vector<vector<int>> &sockets, struct request *req);
void get_request(struct request* req, int s);
void send_request(int s, struct request *req);
void connection_handler(int socket_desc);

