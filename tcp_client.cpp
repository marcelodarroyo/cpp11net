//=============================================================================
// File: tcp_client.cpp
// Description: A stream client using sockets
// Author: Marcelo Arroyo
// Copyright: LGPL
// Changes:
//=============================================================================

#include <unistd.h>      // for read(), write(), close(), ...
#include <stdlib.h>
#include <string.h>      // for strlen
#include <string>        // for string
#include <sys/socket.h>  // for socket(), connect(), ...
#include <arpa/inet.h>   // for struct inet_addr
#include <netdb.h>       // for hostent (DNS search service)

#include "tcp_client.hpp"

using namespace std;

tcp_client::tcp_client() : 
  connected(false)
{}

tcp_client::~tcp_client()
{
  if ( connected )
    disconnect();
}

bool tcp_client::connect(std::string node, std::string service)
{
  struct addrinfo  hints, *result, *rp;
  int found;
  
  // Seach for node and service
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = hints.ai_protocol = 0;
  
  if ( getaddrinfo(node.c_str(), service.c_str(), &hints, &result) != 0 )
    return connected = false;
  
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (s == -1)
       continue;
    if (::connect(s, rp->ai_addr, rp->ai_addrlen) != -1)
       break;
    close(s);
  }
  
  freeaddrinfo(result);
  
  return connected = rp != 0; 
}

void tcp_client::disconnect()
{
  if ( connected ) {
    close(s);
    connected = false;
  }
}

int tcp_client::send(string data)
{
  return write(s,data.data(),data.length());
}

int tcp_client::send(vector<unsigned char> data)
{
  return write(s,data.data(),data.size());
}

vector<unsigned char> tcp_client::receive(int max_length)
{
  vector<unsigned char> result(max_length);
  int n = read(s, &result[0], max_length);
  result.resize(n);
  return result;
}

string tcp_client::receive_string(int max_length)
{
  char buf[max_length];
  int n = read(s, buf, max_length);
  return string(buf,n);
}

// Usage example:
#include <stdio.h>  // for perror()
#include <iostream> // for cout

void error(const char *s)
{
  perror(s);
  exit(1);
}

int main(int argc, char * argv[])
{
  tcp_client c;
  
  if ( argc != 3)
    error("Usage: tcp_client server port");
  if ( !c.connect(argv[1], argv[2]) )
    error("Connect");
  
  c.send("Hola mundo...");
  cout << "Server response: " << c.receive_string() << endl;
  return 0;
}
