//=============================================================================
// File: stream_server_example.cpp
// Description: stream_server example usage
// Author: Marcelo Arroyo (2014)
// Compile command: 
//   g++ -std=c++11 -o stream_server_example stream_server.cpp stream_server_example.cpp
// Changes:
//=============================================================================

#include <iostream>
#include <thread>
#include "stream_server.hpp"

void error(const char * msg)
{
  std::cerr << "Error: " << msg << std::endl;
  exit(1);
}

void do_work(peer_connection c) {
	std::string str = c.receive_string();
    std::cout << "Received: " << str << std::endl;
    c.send("OK");
}

int main(int argc, char * argv[])
{
  stream_server server;
  
  if ( argc != 3 )
    error("Usage: stream_server_example address port");
  
  if ( ! server.bind_to(AF_INET, argv[1], argv[2] ) )
    error("bind");
    
  std::cout << "Listening on " << server.get_node() << ':' << server.get_service() << std::endl;
  
  while (true) {
	peer_connection c = server.accept_connection();
    if ( c.is_connected() ) {
      std::cout << "Connected with " << c.get_peer_node() << ':' 
                << c.get_peer_port() << std::endl;
	  launch_service(c, do_work);
    }
  }
    
  return 0;
}
