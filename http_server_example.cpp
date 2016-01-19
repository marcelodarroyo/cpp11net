//==============================================================================
// http_server_example.cpp
//==============================================================================

#include "http_server.hpp"

int main()
{
	http_server server("localhost", "8080");
	server.run();
	return 0;
}
