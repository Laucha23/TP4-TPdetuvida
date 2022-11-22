# hacer correr al server
g++ -pthread -g -o server server.cpp utils.cpp

# hacer correr al client
g++ -pthread -g -o client client.cpp utils.cpp

# crear a los clients
g++ -pthread -g -o clients creatorClients.cpp utils.cpp