g++ -pthread -g -o client client.cpp utils.cpp
g++ -pthread -g -o server server.cpp utils.cpp
g++ -pthread -g -o clients creatorClients.cpp utils.cpp