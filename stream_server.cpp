//=============================================================================
// File: stream_server.hpp
// Description: Class abstracting a socket server
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
#include <algorithm>     // for std::max
#include <iostream>

#include "stream_server.hpp"

#ifdef UNIX
static int interrupted = 0;
static void signal_handler (int signal_value)
{
    interrupted = 1;
}

static void catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}
#endif

//=======================================================================================
// peer_connection implementation
//=======================================================================================
peer_connection & peer_connection::operator=(peer_connection & other)
{
   s = other.get_socket();
   peer_node = other.get_peer_node();
   peer_port = other.get_peer_port();
   return *this;
}
	
int peer_connection::send(std::vector<unsigned char> data)
{
   return write(s, data.data(), data.size());
}

int peer_connection::send(std::string data)
{
   return write(s, data.c_str(), data.size());
}

std::vector<unsigned char> peer_connection::receive(int max_length)
{
   std::vector<unsigned char> buf(max_length);
   int n = read(s, &(buf[0]), max_length);
   buf.resize(n);
   return buf;
}

std::string peer_connection::receive_string(int max_length)
{
   int size = std::max(max_length, 65000);
   char buf[size];
   int n = read(s, buf, size);
   return std::string(buf, std::max(n,0));
}

//=======================================================================================
// stream_server implementation
//=======================================================================================
bool stream_server::bind_to(sa_family_t family, std::string node, std::string service)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = 0;
    hints.ai_addr = 0;
    hints.ai_next = 0;

    if (getaddrinfo(node.c_str(), service.c_str(), &hints, &result) != 0) 
        return false;
  
    if (s !=- 1) close(s);
  
    for (rp = result; rp != 0; rp = rp->ai_next) {
        s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (s == -1)
            continue;

        if (bind(s, rp->ai_addr, rp->ai_addrlen) == 0) {
	    char node_buf[NI_MAXHOST];
            char service_buf[NI_MAXSERV];
		
            getnameinfo(rp->ai_addr, rp->ai_addrlen, node_buf, NI_MAXHOST,
                        service_buf, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
	    this->node = node_buf; this->service = service_buf;
            break; // success
        }
        close(s);
    }

    freeaddrinfo(result);
  
    if ( listen(s,10) == -1 ) {
        close(s);
        return false;
    }
    
    return rp != 0;
}

peer_connection stream_server::accept_connection()
{
    struct sockaddr_storage peer;
    socklen_t peer_length = sizeof(peer);
    char host[NI_MAXHOST], service[NI_MAXSERV];
  
    host[0] = service[0] = 0; 
    int cs = accept(s, (struct sockaddr *) & peer, &peer_length);
    if ( cs != -1 )
        getnameinfo((struct sockaddr *) &peer, peer_length, host, NI_MAXHOST,
                    service, NI_MAXSERV, NI_NUMERICSERV);
    return peer_connection(cs, host, service);
}

void stream_server::run()
{
#ifdef UNIX
    catch_signals();
#endif
    while(true) {
        if (interrupted) {
            std::cout << "Interrupted..." << std::endl;
            break;
        }
        peer_connection conn = accept_connection();
        if (interrupted) {
            std::cout << "Interrupted..." << std::endl;
            break;
        }
        service_fn(conn);
    }
    disconnect();
}

// Default service function
void stream_server::service_fn(peer_connection & conn)
{
    std::cout << "Connected with" << conn.get_peer_node() << ':' 
              << conn.get_peer_port() << std::endl;
    std::string s = conn.receive_string();
    std::cout << "Received from client:" << s << std::endl;
    conn.send("OK");
}
