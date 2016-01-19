//=============================================================================
// Description: A simple tcp client class
// Author: Marcelo Arroyo (2014)
//=============================================================================

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>       // for string
#include <vector>       // for vector         
#include <arpa/inet.h>  // for struct inet_addr

class tcp_client {

  public:
    tcp_client();
    
    // Destructor: it disconnect if connection is alive
    ~tcp_client();
    
    // connect to a node (ip or name) and service (name or port)
    bool connect(std::string node, std::string service);
    
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
};

#endif
