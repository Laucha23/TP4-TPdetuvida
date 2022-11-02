#include "header.h"

using namespace std;

vector<int> vecinos;
bool estado;


void set_state(int vecinosVivos)
{
	if (estado && vecinosVivos < 2)
	{
		estado = false;
	}
	if (estado && (vecinosVivos == 2 || vecinosVivos == 3))
	{
		estado = true;
	}
	if (estado && vecinosVivos > 3)
	{
		estado = false;
	}
	if (!estado && vecinosVivos == 3)
	{
		estado = true;
	}
}

//Notificamos al server nuestro nuevo estado
void notificarServer(int socketServer)
{
	request reqEstado;
	strncpy(reqEstado.type, "ESTADO", 7);
	strncpy(reqEstado.msg, estado ? "1" : "0", 2);
	send_request(&reqEstado, socketServer);
}

//Aceptamos las conexiones entrantes de los vecinos
int aceptarConexiones(sockaddr_in addr, vector<int> &socketsEscuchar, int clientSocket)
{
	int t = sizeof(addr);
	for (;;)
	{
		int socket = accept(clientSocket, (struct sockaddr *)&addr, (socklen_t *)&t);
		if (socket == -1)
		{
			perror("Error aceptando vecino");
			exit(1);
		}
		socketsEscuchar.push_back(socket);
	}
}

//Cada vez que hay un tick se escucha el estado de los vecinos para setea el nuevo estado de la celula
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

//El cliente envia su estado a sus vecinos 
void notificarVecinos(vector<int> &socketsHablar)
{
	for (int i = 0; i < socketsHablar.size(); ++i)
	{
		request reqEstado;
        strncpy(reqEstado.type, "ESTADO", 7);
        strncpy(reqEstado.msg, estado ? "1" : "0", 2);
        send_request(&reqEstado, socketsHablar[i]);
	}
}

//Por cada puerto genera la conexion con sus vecinos
void conectarVecinos(vector<int> &socketsHablar)
{
	for (int i = 0; i < vecinos.size(); ++i)
	{
		socketsHablar.push_back(connect_socket(vecinos[i]));
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
    int clientPort = atoi(argv[1]);
    int server = connect_socket(PORT);
    int clientSocket = set_acc_socket(clientPort);

    struct sockaddr_in local;

    estado = atoi(argv[1]) % 2 == 0;
    
    //Envia su puerto al servidor
	request reqPuerto;
	strncpy(reqPuerto.type, "PORT", 5);
	strncpy(reqPuerto.msg, to_string(clientPort).c_str(),5);
	send_request(&reqPuerto, server);

    //Envia su estado al servidor
	request reqEstado;
	strncpy(reqEstado.type, "ESTADO", 7);
	strncpy(reqEstado.msg, estado ? "1" : "0", 2);
	send_request(&reqEstado, server);



	vector<thread> threads;

	vector<int> socketsHablar;
	vector<int> socketsEscuchar;

    while (1)
	{
		int socket;
		request reqInfo;
		get_request(&reqInfo, server);
		if (strncmp(reqInfo.type, "VECINOS", 8) == 0)
		{
			getPuertosVecinos(string(reqInfo.msg), vecinos);
			threads.push_back(thread(conectarVecinos, ref(socketsHablar)));
			threads.push_back(thread(aceptarConexiones, local, ref(socketsEscuchar), clientSocket));
		}

		if (strncmp(reqInfo.type, "TICK", 5) == 0)
		{
			threads.push_back(thread(notificarVecinos, ref(socketsHablar)));
			threads.push_back(thread(escucharVecinos, ref(socketsEscuchar), server));
		}
	}

	for (unsigned int i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}

	return 0;

}