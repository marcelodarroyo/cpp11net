all: stream_server_example http_server_example tcp_client

tcp_client: tcp_client.hpp tcp_client.cpp
	g++ -std=c++11 -o $@ tcp_client.cpp
	
stream_server_example: stream_server.o stream_server_example.cpp
	g++ -std=c++11 -o $@ stream_server.o stream_server_example.cpp
	
stream_server.o: stream_server.hpp stream_server.cpp
	g++ -std=c++11 -c stream_server.cpp

http_server.o: http_server.cpp http_server.hpp stream_server.o
	g++ -std=c++11 -c http_server.cpp
	
http_server_example: http_server_example.cpp http_server.o
	g++ -std=c++11 -o $@ http_server_example.cpp http_server.o stream_server.o

clean:
	rm stream_server.o tcp_client stream_server_example
