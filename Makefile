CPP_FLAGS := -DUNIX -DDEBUG -std=c++11 -Wfatal-errors

all: stream_server_example http_server_example tcp_client

tcp_client: tcp_client.hpp tcp_client.cpp
	g++ $(CPP_FLAGS) -o $@ tcp_client.cpp
	
stream_server_example: stream_server.o stream_server_example.cpp
	g++ $(CPP_FLAGS) -o $@ stream_server.o stream_server_example.cpp
	
stream_server.o: stream_server.hpp stream_server.cpp
	g++ $(CPP_FLAGS) -c stream_server.cpp

http_server.o: http_server.cpp http_server.hpp stream_server.o
	g++ $(CPP_FLAGS) -c http_server.cpp
	
http_server_example: http_server_example.cpp http_server.o
	g++ $(CPP_FLAGS) -o $@ http_server_example.cpp http_server.o stream_server.o

clean:
	rm stream_server.o tcp_client stream_server_example
