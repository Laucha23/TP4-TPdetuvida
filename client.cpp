#include "header.h"

using namespace std;


// Asumiendo servidor en ip local, y dado un puerto, establece una conexion
// con el destino retornando un socket en estado conectado en caso de exito
int connect_socket(int port)
{
   // TO DO
}


// Dada una lista de puertos de vecinos, conecta el cliente con cada vecino
// agregando cada socket al vector de sockets
void con2neigh(string list, vector<int>& sockets)
{
    // TO DO
}

// Dado el estado actual de la celula y el estado de los vecinos en una ronda
// computa el nuevo estado de la celula segun las reglas del juego
bool set_state(bool alive, const vector<request>& cl)
{
	// TO DO
}

int run_cell(int port)
{   
    char                buf[MENSAJE_MAXIMO+1];
    struct request      srv_req;
    int                 srv_socket, accepting_socket;
    // Definir estructuras para manejar los sockets
    // Sugerancia: Diferenciar los canales donde la celula publica su estado
    //             de los que usa para recibir estado de sus vecinos
  
    /* Conectarse al server */
    srv_socket = connect_socket(htons(port));

    /* Crear socket de escucha y permitir aceptar conexiones concurrentemente */
    int lsn_port = /* TO DO*/ ;
    acc_sock_fd = /* TO DO*/
    /* TO DO*/
  
    /* Enviar msg al srv con el puerto de escucha */
    /* TO DO*/
    
    /* Obtener lista de vecinos inicial */
    /* TO DO*/

    /* Conectarse a los vecinos */
    /* TO DO*/ 

    /* Enviar msg ready para el server */
    /* TO DO*/

    /* Comenzar juego */
    srand(getpid());
    char alive = random() % 2;
    while(1)
    {
        // Esperar request del srv
        get_request(&srv_req, srv_socket);
        if (strncmp(srv_req.type,"TICK",4) == 0)
        {
            /* Publicar estado*/
            /* TO DO*/

            /* Obtener estado de los vecinos*/
            /* TO DO*/

            /* Computar nuevo estado*/
            /* TO DO*/

            /* Informar al srv nuevo estado*/
            /* TO DO*/
        }
        else if (strncmp(srv_req.type,"NEW",3) == 0)
        {
            /* Conectarse a los nuevos vecinos*/
            /* TO DO*/

            /* Avisar con CLIENT_READY al srv*/
            /* TO DO*/

        } 

    }
    
    return;
}


int main(int argc, char* argv[]){
    int pid;
    /* Lanzar tantos procesos celulas como los indicados por argv[1]*/
    /* TO DO*/
    
}