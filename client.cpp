#include "header.h"

using namespace std;

vector<int> vecinos;
bool estado;

void cambiarEstado(int vecinosVivos)
{
	if (estado && vecinosVivos < 2){
		estado = false;
	}
	if (estado && (vecinosVivos == 2 || vecinosVivos == 3)){
		estado = true;
	}
	if (!estado && vecinosVivos == 3){
		estado = true;
	}
	if (estado && vecinosVivos > 3){
		estado = false;
	}
}

// Se notifica al servidor con el nuevo estado
void notificarSV(int socketServer)
{
	request requestEstado;
	strncpy(requestEstado.type, "ESTADO", 7);
	strncpy(requestEstado.msg, estado ? "1" : "0", 2);
	send_request(&requestEstado, socketServer);
}

// Se aceptan las conexiones pendientes de los vecinos
int aceptarConexiones(sockaddr_in address, vector<int> &escucharSocket, int clienteSocket)
{
	int t = sizeof(address);
	for (;;)
	{
		int socket = accept(clienteSocket, (struct sockaddr *)&address, (socklen_t *)&t);
		if (socket == -1)
		{
			perror("ERROR: conexión no aceptada");
			exit(1);
		}
		escucharSocket.push_back(socket);
	}
}

// Se escucha el estado de los vecinos por cada tick
void escucharVecinos(vector<int> &socketsVecinos, int serverSocket)
{
	int vecinosVivos = 0;
	for (int i = 0; i < socketsVecinos.size(); ++i)
	{
		request request;
        get_request(&request, socketsVecinos[i]);
		if (strncmp(request.msg, "1", 2) == 0)
        {
            vecinosVivos++;
        }
    }

	cambiarEstado(vecinosVivos);
	notificarSV(serverSocket);
}

// Cliente envia su estado a vecinos
void notificarVecinos(vector<int> &hablarSocket)
{
	for (int i = 0; i < hablarSocket.size(); ++i)
	{
		request requestEstado;
        strncpy(requestEstado.type, "ESTADO", 7);
        strncpy(requestEstado.msg, estado ? "1" : "0", 2);
        send_request(&requestEstado, hablarSocket[i]);
	}
}

// Se genera la conexión por cada puerto, con sus vecinos
void conectarVecinos(vector<int> &hablarSocket)
{
	for (int i = 0; i < vecinos.size(); ++i)
	{
		hablarSocket.push_back(conectarSocket(vecinos[i]));
	}
}

void getPuertosVecinos(string puertosVecinos, vector<int> &puertos)
{
	const char separador = '-';
	stringstream stringstream(puertosVecinos);

	string s;
	while (std::getline(stringstream, s, separador))
	{
		if (s != "")
		{
			puertos.push_back(atoi(s.c_str()));
		}
	}
}

int main(int argc, char* argv[]){
    int clientePort = atoi(argv[1]);
    int server = conectarSocket(PORT);
    int clienteSocket = set_acc_socket(clientePort);

    struct sockaddr_in local;

    estado = atoi(argv[1]) % 2 == 0;
    
    // Enviar puerto al servidor
	request requestPuerto;
	strncpy(requestPuerto.type, "PORT", 5);
	strncpy(requestPuerto.msg, to_string(clientePort).c_str(),5);
	send_request(&requestPuerto, server);

    // Enviar estado al servidor
	request requestEstado;
	strncpy(requestEstado.type, "ESTADO", 7);
	strncpy(requestEstado.msg, estado ? "1" : "0", 2);
	send_request(&requestEstado, server);


	vector<thread> threads;
	vector<int> hablarSocket;
	vector<int> escucharSocket;


    while (1)
	{
		int socket;
		request requestInfo;
		get_request(&requestInfo, server);
		if (strncmp(requestInfo.type, "VECINOS", 8) == 0)
		{
			getPuertosVecinos(string(requestInfo.msg), vecinos);
			threads.push_back(thread(conectarVecinos, ref(hablarSocket)));
			threads.push_back(thread(aceptarConexiones, local, ref(escucharSocket), clienteSocket));
		}
		if (strncmp(requestInfo.type, "TICK", 5) == 0)
		{
			threads.push_back(thread(notificarVecinos, ref(hablarSocket)));
			threads.push_back(thread(escucharVecinos, ref(escucharSocket), server));
		}
	}

	for (unsigned int i = 0; i < threads.size(); i++){
		threads[i].join();
	}

	return 0;
}