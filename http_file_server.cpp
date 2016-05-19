//==============================================================================
// http_server_example.cpp
//==============================================================================

#include <iostream>
#include "http_server.hpp"

int main()
{
	http_server server("192.168.128.118", "8080");
	std::cout << "Listening on port 8080...\n";
	server.run();
	return 0;
}
