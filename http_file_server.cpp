//==============================================================================
// http_server_example.cpp
//==============================================================================

#include <iostream>
#include "http_server.hpp"

int main(int argc, char * argv[])
{
    if ( argc < 2 ) {
        std::cerr << "Usage: " << argv[0] << " host [port]" << std::endl;
        return -1;
    }
    std::string port = "8080";
    if ( argc == 3 )
        port = argv[2];
       
    http_server server(argv[1], port);
    if ( server.ready() ) {
        std::cout << "Listening on " << argv[1] << ':' << port << " ..." << std::endl;
        server.run();
    }
    else
        std::cerr << "Error binding address." << std::endl;
    return 0;
}

// vim: tabstop=4 expandtab:
