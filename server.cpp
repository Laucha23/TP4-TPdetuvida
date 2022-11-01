#include "header.h"
using namespace std;

// Sugerencia: El servidor va a necestiar varios threads. Recordar que peuden
// compartir variables y que por lo tanto las herramientas de sincronziacion
// como semaforos son perfectamente validas.


// Servicio draw: En cada tick, imprime el mapa con el estado de cada celula 
void draw()
{
    /* TO DO*/
}

// Servicio timer: Cada cierto intervalo de tiempo publica un tick. 
//Es importante que cada tick se envie utilizando el mapa de vecinos actualizado

void timer()
{
   /* TO DO*/
}



// Thread map_creator: Agrega los nuevos nodos al mapa
void map_creator(/* TO DO*/)
{
  
    /* Registrar los lsn ports de los nuevos */
    /* TIP: Hay que esperar que los clientes manden el mensaje con el lsn port*/
    /* Varias formas de hacerlo, pselect puede resultar comodo para este caso */
    /* Crear threads podria ser demasiado overhead, pero es valido */
    /* TO DO*/

    // Avisar a las celulas que correspondan la nueva estructura de vecinos
    // TIP: Puede ser util separar el caso inicial del resto, sobre todo para
    //      facilitar luego el testeo
    if(N ==3){
        base_case_3x3(/* TO DO*/);
        return;
    }
    general_case_nxn(/* TO DO*/);

}

void server_accept_conns(int s)
{
    while(1)
    {
        /* Acpetar nueva celula*/
        /* TO DO*/
        
        /* Si ya hay suficientes para armar matriz de 3x3 o para agregar L*/
        /* Actualizar el mapa permitiendo que sigan llegando conexiones */
        /* Sugerencia: Lanzar thread pmap_creator

        /* Si no, marcarlas como pendientes y continuar*/
        /* TO DO*/   

    }
}

int main(int argc, char* argv[])
{
    int s;
    thread ths[MAX_CLIENTS];
    s = set_acc_socket(atoi(argv[1]));

    /* Levantar servicios y aceptar conexiones */
   /* TO DO*/

    return 0;
}

