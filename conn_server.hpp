//=============================================================================
// Description: A simple tcp client class
// Author: Marcelo Arroyo (2014)
// To do: modify to work with different socket types (unix, internet,...)
//=============================================================================

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>       // for string
#include <vector>       // for vector         
#include <arpa/inet.h>  // for struct inet_addr

class stream_server {

  public:
    stream_server();
    
    // Destructor: it disconnect if connection is alive
    ~stream_server();
    
    // connect to a node (ip or name) and service (name or port)
    bool bind_to(std::string node, std::string service);
    void accept_connection();
    
    str::string get_peer_node() const { return peer_node; }
    str::string get_peer_port() const { return peer_port; }
    
    // close connection
    void disconnect();
    
    // send data, return bytes sent
    int send(std::vector<unsigned char> data);
    int send(std::string data);
    
    // receive data
    std::vector<unsigned char> receive(int max_length=4096);
    std::string receive_string(int max_length=4096);
    
    inline bool is_connected() const { return connected; }
    
  protected:
    int s;                        // the socket
    bool connected;               // Is the connection alive?
    str::string peer_node;
    str::string peer_port;
};

#endif
