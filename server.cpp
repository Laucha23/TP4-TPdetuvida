#include "header.h"
using namespace std;

// Sugerencia: El servidor va a necestiar varios threads. Recordar que peuden
// compartir variables y que por lo tanto las herramientas de sincronziacion
// como semaforos son perfectamente validas.

// Lista sockets-clientes en tablero
vector<vector<int>> socketsClientes(HOR, vector<int>(VER));
// Lista puertos-clientes
vector<vector<int>> puertosClientes(HOR, vector<int>(VER));
// Lista de los sockets pre-juego
vector<int> socketsListos;

// Lista ubicaciones
vector<vector<int>> ubicacionesVecinos{
vector<int>{1, 1},
vector<int>{0, 1},
vector<int>{-1, 1},
vector<int>{1, -1},
vector<int>{1, 0},
vector<int>{-1, 0},
vector<int>{0, -1},
vector<int>{-1, -1}};

// Variable que dice si el juego está corriendo o no
bool running;

// Chequea que la posicion sea valida y la agrega al array de vecinos
void calcularUbicacionVecino(int x, int y, int i, vector<vector<int>> &vecinos)
{
        int xVecino = x + ubicacionesVecinos[i][0];
        int yVecino = y + ubicacionesVecinos[i][1];
        if (xVecino > -1 && yVecino > -1 && xVecino < socketsClientes.size() && yVecino < VER){
            vecinos.push_back(vector<int>{xVecino, yVecino});
        }
}

// Se calculan las posiciones de los posibles vecinos
vector<vector<int>> getVecinos(int x, int y)
{
	vector<vector<int>> vecinos;
    for(size_t i = 0; i < 8; i++){
        calcularUbicacionVecino(x, y, i, vecinos);
    }
	return vecinos;
}

// Mensaje para que el cliente conozca sus vecinos
string mensajeVecinos(vector<vector<int>> &vecinos)
{
	string vecinosString = "";

	for (int i = 0; i < vecinos.size(); ++i){
		vecinosString += ".";
		vecinosString += to_string(puertosClientes[vecinos[i][0]][vecinos[i][1]]);
	}
	return vecinosString;
}

// Se notifica a los vecinos de cada cliente
void notificarClientes(sem_t& sem){
	for (size_t i = 0; i < socketsClientes.size(); i++){
		for (size_t j = 0; j < socketsClientes[i].size(); j++){
			vector<vector<int>> vecinos = getVecinos(i, j);
			string vecinosString = mensajeVecinos(vecinos);
			request req;
			strncpy(req.type, "VECINOS", 8);
			strncpy(req.msg, vecinosString.c_str(), MENSAJE_MAXIMO);
			send_request(socketsClientes[i][j], &req);
		}
	}
    sem_post(&sem);
}

void esperarNuevoJuego(sem_t& semaforo)
{
    int cantidadCeldas = 0; 
    while(!running)
    {
        sleep(5);
        cantidadCeldas = socketsClientes.size() * VER;

        int contador = VER * socketsClientes.size();

        if(( socketsListos.size() - cantidadCeldas) / VER > 0)
        {
            for (size_t i = 0; i < (socketsListos.size() - cantidadCeldas) / VER; i++)
            {
                vector<int> nuevaFila;
                vector<int> nuevaFilaPuertos;
                for (size_t j = 0; j < VER; j++)
                {
                    nuevaFila.push_back(socketsListos[contador]);
                    request requestCliente;
                    get_request(&requestCliente, socketsListos[contador]);
                    char puerto[sizeof(requestCliente.msg)];
                    strncpy(puerto, requestCliente.msg, sizeof(requestCliente.msg));
                    
                    nuevaFilaPuertos.push_back(atoi(puerto));
                    contador++;
                }
                socketsClientes.push_back(nuevaFila);
                puertosClientes.push_back(nuevaFilaPuertos);
            }
            sleep(5);
            sem_t sem;
            sem_init(&sem, 0, 0);
            notificarClientes(ref(sem));
            sem_wait(&sem);

            running = true;
        }
    }
    sem_post(&semaforo);
}

// Servicio draw: En cada tick, imprime el mapa con el estado de cada celula 
void draw()
{
    cout << "TABLERO : " << endl;
    string tablero = "";
    int contadorVidas = 0;
	for (size_t i = 0; i < socketsClientes.size(); i++)
    {
		tablero+= "\n";
        for (size_t j = 0; j < socketsClientes[i].size(); j++)
        {
			request reqEstado;
            get_request(&reqEstado, socketsClientes[i][j]);
			tablero+= " ";
			tablero+= reqEstado.msg;
			tablero+= " ";
            if(strncmp(reqEstado.msg, "1", 2) == 0){
                contadorVidas++;
            }
        }   
        tablero+= "\n";
    }
    if(contadorVidas > 0){
        cout << tablero << endl;
    }
    else{
        cout << tablero << endl;
    	cout << "NO QUEDAN VIVOS" << endl;
        running = false;

        sem_t semaforoNuevoJuego;
	    sem_init(&semaforoNuevoJuego, 0, 0);

        thread t = thread(esperarNuevoJuego, ref(semaforoNuevoJuego));
        t.join();
        sem_wait(&semaforoNuevoJuego);
    }
}

// Servicio timer: Cada cierto intervalo de tiempo publica un tick. 
// Es importante que cada tick se envie utilizando el mapa de vecinos actualizado
void timer()
{
	int contador = 0;
    system("clear");
    cout << "Inicio del juego" << endl;
    running = true;
	while (1)
	{
		draw();
		string tick = "TIEMPO: " + to_string(contador);
		char tiempo[tick.length() + 1];
		strcpy(tiempo, tick.c_str());
		request req;
		strncpy(req.msg, tiempo, sizeof(tiempo));
		strncpy(req.type, "TICK", 5);
		broadcast(socketsClientes, &req);
		contador++;
		sleep(5);
	}
}

// Esperar aceptación conexiones entrantes
void server_accept_conns(int s, sem_t& semaforo)
{
    struct sockaddr_in remote;
	int t = sizeof(remote);
	int socket;
	for (size_t i = 0; i < HOR; i++)
	{
        for (size_t j = 0; j < VER; j++)
        {
            if ((socket = accept(s, (struct sockaddr *)&remote, (socklen_t *)&t)) == -1)
            {
                perror("ERROR: aceptación cliente");
                exit(1);
            }
            socketsListos.push_back(socket);
            sem_post(&semaforo);
        }
	}  
}

// Esperar aceptación nuevas conexiones entrantes
void server_accept_new_conns(int s)
{
    struct sockaddr_in remote;
	int t = sizeof(remote);
	int socket;
	for (;;)
	{
        if ((socket = accept(s, (struct sockaddr *)&remote, (socklen_t *)&t)) == -1)
        {
            perror("ERROR: aceptación cliente");
            exit(1);
        }
        socketsListos.push_back(socket);
	}  
}

// Espera recepción todos los puertos
void server_get_ports(sem_t& semaforo)
{
    for (size_t i = 0; i < HOR; i++)
        {
            for (size_t j = 0; j < VER; j++)
            {
                request requestCliente;
                get_request(&requestCliente, socketsClientes[i][j]);
                char puerto[sizeof(requestCliente.msg)];
                strncpy(puerto, requestCliente.msg, sizeof(requestCliente.msg));
                puertosClientes[i][j] = atoi(puerto);
                sem_post(&semaforo);
            }
        }
}

bool llenarLista(){
    if (socketsListos.size() == VER * HOR){
		int contador = 0;
		for (size_t i = 0; i < HOR; i++){
			for (size_t j = 0; j < VER; j++){
				socketsClientes[i][j] = socketsListos[contador];
				contador++;
			}
		}
		return true;
	}
	return false;
}

int main(void)
{
    int s;
    struct sockaddr_in local;
    struct sockaddr_in remote;
    vector <thread> threads;

    sem_t semaforoConexiones;
	sem_init(&semaforoConexiones, 0, 0);
    sem_t semaforoPorts;
	sem_init(&semaforoPorts, 0, 0);

    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("ERROR: socket server");
        exit(1);
    }

    local.sin_family = AF_INET;
    local.sin_port = htons(PORT);
    local.sin_addr.s_addr = INADDR_ANY;

    int localLink = bind(s, (struct sockaddr *)&local, sizeof(local));
    if (localLink < 0) {
        perror("ERROR: bind");
        exit(1);
    }

    int listenMode = listen(s, 10);
    if (listenMode == -1) {
        perror("ERROR: listen server");
        exit(1);
    }

    server_accept_conns(s, ref(semaforoConexiones));

    for (size_t i = 0;  i < VER * HOR; i++){
		sem_wait(&semaforoConexiones);
	}
    if (llenarLista()){
        server_get_ports(ref(semaforoPorts));

         for (size_t i = 0;  i < VER * HOR; i++){
            sem_wait(&semaforoPorts);
        }

        sleep(5);

        sem_t sem;
        sem_init(&sem, 0, 0);
        notificarClientes(ref(sem));
        sem_wait(&sem);

        threads.push_back(thread(timer));
        threads.push_back(thread(server_accept_new_conns, s));
    }

    for (unsigned int i = 0; i < threads.size(); i++){
        threads[i].join();
    }
    
    close(s);
    return 0;
}