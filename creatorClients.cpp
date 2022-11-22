#include "header.h"

// Llamada al cliente con un numero random
void cliente(int random){
    std::string command = "./client ";
    command += std::to_string(random);
    system(command.c_str());
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    int max = 9000;
    int min = 1000;
    int pid = -1;

    // Creación de los clientes minimos para iniciar el juego
    for (size_t i = 0; i < HOR * VER; i++){
        pid = -1;
        int random = rand()% max + min; 
        pid = fork();
        if (pid == 0) {
            cliente(random);
            return 0;
        }
    }
    if (pid > 0){
        for (size_t i = 0; i < HOR * VER; i++) {
            wait(NULL);
        }
        exit(0);
    }
    
    return 0;
}