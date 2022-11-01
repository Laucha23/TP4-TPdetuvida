g++ -pthread -g -o cliente client.cpp utils.cpp
g++ -pthread -g -o server server.cpp utils.cpp
g++ -pthread -g -o clientes crearClientes.cpp utils.cpp