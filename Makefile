CPP_FLAGS := -DUNIX -DDEBUG -std=c++11 -Wfatal-errors

all: stream_server_example http_file_server tcp_client

tcp_client: tcp_client.hpp tcp_client.cpp
	g++ $(CPP_FLAGS) -o $@ tcp_client.cpp
	
stream_server_example: stream_server.o stream_server_example.cpp
	g++ $(CPP_FLAGS) -o $@ stream_server.o stream_server_example.cpp
	
stream_server.o: stream_server.hpp stream_server.cpp
	g++ $(CPP_FLAGS) -c stream_server.cpp

http_server.o: http_server.cpp http_server.hpp http_request.hpp http_request.cpp http_service.hpp http_service.cpp stream_server.o
	g++ $(CPP_FLAGS) -c http_server.cpp http_request.cpp http_service.cpp
	
http_file_server: http_file_server.cpp http_server.o
	g++ $(CPP_FLAGS) -o $@ http_file_server.cpp http_server.o stream_server.o http_request.o http_service.o

clean:
	rm stream_server.o tcp_client stream_server_example
