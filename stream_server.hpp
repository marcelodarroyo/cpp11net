//=============================================================================
// Description: A stream socket server
// Author: Marcelo Arroyo (2014)
// To do: modify to work with different socket types (unix, internet,...)
//=============================================================================

#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include <string>       // for string
#include <vector>       // for std::vector         
#include <arpa/inet.h>  // for struct inet_addr
#include <unistd.h>
#include <signal.h>     // for signal()

//=============================================================================
// Objects of this class represents a connected socket with a peer node.
// They are returned by stream_server::accept_connection()
//=============================================================================
class peer_connection {
public:
	
    peer_connection() : s(-1) {}
    peer_connection(int socket, std::string peer_node, std::string peer_port)
        : s(socket), peer_node(peer_node), peer_port(peer_port)
    {}
		
    peer_connection(const peer_connection & other)
        : s(other.get_socket()), peer_node(other.get_peer_node()), peer_port(other.get_peer_port())
    {}
	
    ~peer_connection() { disconnect(); }
	
    peer_connection & operator=(peer_connection & other);
	
    int get_socket() const { return s; }
    std::string get_peer_node() const { return peer_node; }
    std::string get_peer_port() const { return peer_port; }
    
    // send data, return bytes sent
    int send(std::vector<unsigned char> data);
    int send(std::string data);
    
    // receive data
    std::vector<unsigned char> receive(int max_length=4096);
    std::string receive_string(int max_length=4096);
	
    // close connection
    void disconnect() { if (s != -1) close(s); }
	
	bool is_connected() { return s != -1; }
	
protected:
	int s;
	std::string peer_node, 
	            peer_port;
};

//=============================================================================
// A simple stream (connection oriented) server
//=============================================================================
class stream_server {

public:
    
	// Families suported: AF_UNIX, AF_INET, ...
    stream_server() : s(-1) {}
    
    // Destructor: it disconnect if connection is alive
    virtual ~stream_server() { disconnect(); }
    
    // connect to a node (ip or name) and service (name or port)
    bool bind_to(sa_family_t family, std::string node, std::string service);
    
	// wait for connection request and return a connected socket
    peer_connection accept_connection();
	
    std::string get_node() { return node; }
    std::string get_service() { return service; }
	
    void disconnect() { if (s != -1) { close(s); s=-1; } }
	
protected:
    int s; 			// socket
        std::string node,       // binded node (address)
                    service;	// binded service (port)
};

//=============================================================================
// Utilities
//=============================================================================
using service_fun_t = void(*)(peer_connection);
	
void launch_service(peer_connection & c, service_fun_t service);
#endif
