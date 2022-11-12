#include "header.h"
using namespace std;

// Lista de puertos vecinos
vector<int> listaVecinos;
// Estado de vida de la celda
bool estadoCelda;

// Socket del cliente
int cSocket;
int puertoCliente;


// Define el nuevo estado de la celula segun los vecinos y sus estados
void set_state(int vecinosVivos)
{
	string separador = "";
	int cantidadVecinos = listaVecinos.size();
	
	for (int i = 0; i < cantidadVecinos; ++i){
		separador += ".";
		separador += to_string(listaVecinos[i]);
	}
	if (vecinosVivos > 3){
		estadoCelda = false;
	} else if (vecinosVivos < 2){
		estadoCelda = false;
	} else if (vecinosVivos == 3){
		estadoCelda = true;
	} else if (vecinosVivos == 2 && estadoCelda){
		estadoCelda = true;
	}
}

// Notificamos al server nuestro nuevo estado
void notificarServer(int socketServer)
{
	request reqEstado;
	strncpy(reqEstado.type, "ESTADO", 7);
	strncpy(reqEstado.msg, estadoCelda ? "1" : "0", 2);
	send_request(socketServer, &reqEstado);
}

// Aceptamos las conexiones entrantes de los vecinos
void aceptarConexiones(sockaddr_in addr, vector<int> &escucharSockets)
{
	int t = sizeof(addr);
	for (;;)
	{
		int socket = accept(cSocket, (struct sockaddr *)&addr, (socklen_t *)&t);
		if (socket == -1)
		{
			perror("ERROR: conexión no aceptada");
			exit(1);
		}
		escucharSockets.push_back(socket);
	}
}

// Conectarse a un vecino
int conectarVecino(int puertoCliente)
{
	struct sockaddr_in remote;
	int vecino;
	if ((vecino = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("ERROR: creacion socket vecino");
		exit(1);
	}
	remote.sin_family = AF_INET;
	remote.sin_port = htons(puertoCliente);
	remote.sin_addr.s_addr = INADDR_ANY;
	int s = connect(vecino, (struct sockaddr *)&remote, sizeof(remote));
	if (s == -1)
	{
		perror("ERROR: conectar a vecino");
		exit(1);
	}

	return vecino;
}

// Por cada tick, se ve el estado de los vecinos (Para setear los nuevos estados)
void escucharVecinos(vector<int> &socketsVecinos, int serverSocket)
{
	int vecinosVivos = 0;
	for (int i = 0; i < socketsVecinos.size(); ++i)
	{
		request req;
        get_request(&req, socketsVecinos[i]);
		if (strncmp(req.msg, "1", 2) == 0)
        {
            vecinosVivos++;
        }
    }

	set_state(vecinosVivos);
	notificarServer(serverSocket);
}

// Cliente notifica a sus vecinos
void notificarVecinos(vector<int> &socketsHablar, sem_t& semaforo)
{
	for (int i = 0; i < socketsHablar.size(); ++i)
	{
		request reqEstado;
        strncpy(reqEstado.type, "ESTADO", 7);
        strncpy(reqEstado.msg, estadoCelda ? "1" : "0", 2);
        send_request(socketsHablar[i], &reqEstado);
	}
	sem_post(&semaforo);
}

// Se genera la conexion con vecinos por puerto
void conectarVecinos(vector<int> &socketsHablar)
{
	for (int i = 0; i < listaVecinos.size(); ++i)
	{
		socketsHablar.push_back(conectarVecino(listaVecinos[i]));
	}
}

void getPuertosVecinos(string puertosVecinos, vector<int> &puertos)
{
	const char separador = '-';
	stringstream ss(puertosVecinos);

	string s;
	while (std::getline(ss, s, separador))
	{
		if (s != "")
		{
			puertos.push_back(atoi(s.c_str()));
		}
	}
}

int main(int argc, char* argv[]){
    int pid;

    struct sockaddr_in remote;
	struct sockaddr_in local;
	struct hostent *hp;
	struct in_addr addr;
	char buf[MENSAJE_MAXIMO];

    // Recibe un numero aleatorio por el cual  se decide si la celda esta viva o muerta
	estadoCelda = atoi(argv[1]) % 2 == 0;

	vector<thread> threads;

	vector<int> socketsHablar;
	vector<int> escucharSockets;

	sem_t semafotoHablar;
	sem_t semaforoEscuchar;
	sem_init(&semafotoHablar, 0, 0);
	sem_init(&semaforoEscuchar, 0, 0);


    cSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (cSocket == -1)
	{
		perror("ERROR: socket cliente");
		exit(1);
	}

	puertoCliente = atoi(argv[1]);
	local.sin_family = AF_INET;
	local.sin_port = htons(puertoCliente);
	local.sin_addr.s_addr = INADDR_ANY;

    int localLink = bind(cSocket, (struct sockaddr *)&local, sizeof(local));
	if (localLink < 0)
	{
		perror("ERROR: bind");
		exit(1);
	}

    int listenMode = listen(cSocket, 10);
	if (listenMode == -1)
	{
		perror("ERROR: listen server");
		exit(1);
	}

	int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_fd  == -1)
	{
		perror("ERROR: socket server");
		exit(1);
	}

	remote.sin_family = AF_INET;
	remote.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &(remote.sin_addr));

	int s = connect(socket_fd, (struct sockaddr *)&remote, sizeof(remote));
	if (s == -1)
	{
		perror("ERROR: conexion server");
		exit(1);
	}

	// Envia puerto al servidor
	request reqPuerto;
	strncpy(reqPuerto.type, "PORT", 5);
	strncpy(reqPuerto.msg, to_string(puertoCliente).c_str(),5);
	send_request(socket_fd, &reqPuerto);

    // Envia estado al servidor
	request reqEstado;
	strncpy(reqEstado.type, "ESTADO", 7);
	strncpy(reqEstado.msg, estadoCelda ? "1" : "0", 2);
	send_request(socket_fd, &reqEstado);

	while (1)
	{
		int socket;
		request reqInfo;
		get_request(&reqInfo, socket_fd);
        // Server notifica los puertos vecinos del cliente
		if (strncmp(reqInfo.type, "VECINOS", 8) == 0){
			listaVecinos.clear();
			socketsHablar.clear();
			escucharSockets.clear();	
			estadoCelda = atoi(argv[1]) % 2 == 0;

			getPuertosVecinos(string(reqInfo.msg), listaVecinos);
			threads.push_back(thread(conectarVecinos, ref(socketsHablar)));
			threads.push_back(thread(aceptarConexiones, local, ref(escucharSockets)));
		}

        // Marca de los ticks
		if (strncmp(reqInfo.type, "TICK", 5) == 0){
			threads.push_back(thread(notificarVecinos, ref(socketsHablar), ref(semafotoHablar)));
			sem_wait(&semafotoHablar);
			threads.push_back(thread(escucharVecinos, ref(escucharSockets), socket_fd));
		}
	}

	for (unsigned int i = 0; i < threads.size(); i++){
		threads[i].join();
	}

	return 0;
}