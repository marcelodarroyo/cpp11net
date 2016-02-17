//=============================================================================
// File: stream_server_example.cpp
// Description: stream_server example usage
// Author: Marcelo Arroyo (2014)
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

class my_server : public stream_server {
public:
    void service_fn(peer_connection & conn) {
        // call default service function
        stream_server::service_fn(conn);
    }
};

int main(int argc, char * argv[])
{
      my_server server;
      
      if ( argc != 3 )
        error("Usage: stream_server_example address port");
      
      if ( ! server.bind_to(AF_INET, argv[1], argv[2] ) )
        error("bind");
        
      std::cout << "Listening on " << server.get_node() << ':' 
                << server.get_service() << std::endl;
     
      server.run();
        
      return 0;
}
