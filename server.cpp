#include "header.h"
using namespace std;

// Sugerencia: El servidor va a necestiar varios threads. Recordar que peuden
// compartir variables y que por lo tanto las herramientas de sincronziacion
// como semaforos son perfectamente validas.
vector<vector<int>> ubicacionesVecinos{vector<int>{1, 1},
vector<int>{0, 1},
vector<int>{-1, 1},
vector<int>{1, -1},
vector<int>{1, 0},
vector<int>{-1, 0},
vector<int>{0, -1},
vector<int>{-1, -1}};


// Servicio draw: En cada tick, imprime el mapa con el estado de cada celula 
void draw(vector<vector<int>> &matrix)
{
    cout << "Tablero: " << endl;
    string tablero = "";
	for (size_t i = 0; i < 3; i++)
    {
		tablero+= "\n";
        for (size_t j = 0; j < 3; j++)
        {
			request requestEstado;
            get_request(&requestEstado, matrix[i][j]);
			tablero += " ";
			tablero += requestEstado.msg;
			tablero += " ";
        }   
        tablero += "\n";
    }
}


// Servicio timer: Cada cierto intervalo de tiempo publica un tick. 
// Es importante que cada tick se envie utilizando el mapa de vecinos actualizado
void timer(vector<vector<int>> &matrix)
{
    int contador = 0;
    cout << "Comienza el juego" << endl;

	while (1)
	{
		draw(ref(matrix));
		string tick = "Tiempo " + to_string(contador);
		char tiempo[tick.length() + 1];
		strcpy(tiempo, tick.c_str());
		request request;
		strncpy(request.msg, tiempo, sizeof(tiempo));
		strncpy(request.type, "TICK", 5);
		broadcast(matrix, &request);
		contador++;
		sleep(2);
	}
}


void mandarVecinos(vector<vector<int>> &matrix, vector<vector<int>> &puertos)
{
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
            //Se calcula las posiciones de los vecinos de cada casilla
            vector<vector<int>> vecinosUbicacion;
            for(size_t i = 0; i < 8; i++){
                int xVecino = i + ubicacionesVecinos[i][0];
                int yVecino = j + ubicacionesVecinos[i][1];
                if (xVecino > -1 && yVecino > -1 && xVecino < 3 && yVecino < 3)
                {
                    vecinosUbicacion.push_back(vector<int>{xVecino, yVecino});
                }
            }
            //Se genera un string con los puertos de los clientes vecinos
            string vecinosString = "";
            for (int i = 0; i < vecinosUbicacion.size(); ++i)
            {
                vecinosString += "puerto";
                vecinosString += to_string(puertos[vecinosUbicacion[i][0]][vecinosUbicacion[i][1]]);
            }
            //Se envia la info a cada cliente
			request request;
			strncpy(request.type, "VECINOS", 8);
			strncpy(request.msg, vecinosString.c_str(), MENSAJE_MAXIMO);
			send_request(&request, matrix[i][j]);
		}
	}
}


void server_accept_conns(int s, vector<int>& sockets, vector<vector<int>> &matrix, sem_t& semaforo)
{
	for (size_t i = 0; i < 3;i++)
	{
        for (size_t j = 0; j < 3;j++)
        {
            accept_conns(s, ref(sockets), semaforo);
        }
	}  
    if(sockets.size() == 9){
        int contador = 0;
        for (size_t i = 0; i < 3;i++)
        {
            for (size_t j = 0; j < 3;j++)
            {
                matrix[i][j] = sockets[contador];
                contador++;
            }
        }  
    }
}

int main(int argc, char* argv[])
{
    int s;
    vector<thread> threads;
	vector<int> sockets;
	vector<vector<int>> matrix(3, vector<int>(3));
	vector<vector<int>> puertos(3, vector<int>(3));

    sem_t semaforo;
	sem_init(&semaforo, 0, 0);

    s = set_acc_socket(PORT);

    threads.push_back(thread(server_accept_conns, s, ref(sockets), ref(matrix), ref(semaforo)));

   if(matrix.size() == 9){
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                request request;
                get_request(&request, matrix[i][j]);
                char puerto[sizeof(request.msg)];
                strncpy(puerto, request.msg, sizeof(request.msg));
                puertos[i][j] = atoi(puerto);

            }
        }

        mandarVecinos(ref(matrix), ref(puertos));
        threads.push_back(thread(timer,ref(matrix)));
   }

    for (unsigned int i = 0; i < threads.size(); i++)
    {
        threads[i].join();
    }

    close(s);

    return 0;
}

