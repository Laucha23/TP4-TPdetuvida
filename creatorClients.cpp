#include "header.h"

// Llamada al cliente con un numero random
void client(int random){
    std::string command = "./client ";
    command += std::to_string(random);
    system(command.c_str());
}

int main(int argc, char const *argv[])
{
    vector <thread> threads;

    srand(time(NULL));

    // Creación de los clientes minimos para iniciar el juego
    for (size_t i = 0; i < 9; i++){
        threads.push_back(thread(client, (rand()% 10000)));
    }
    for (unsigned int i = 0; i < 9; i++){
		threads[i].join();
	}
    
    return 0;
}