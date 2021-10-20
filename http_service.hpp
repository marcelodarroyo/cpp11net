#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include <string>
#include <map>
#include <fstream>
#include "stream_server.hpp"
#include "http_request.hpp"

//=============================================================================
// An http service (base class). Default behaviour: serve static files with
// uploading and files listing support
//=============================================================================
class http_service {

public:
    http_service(std::string default_files_dir = ".")
        : files_dir(default_files_dir)
        {}
    
    // HTTP supported commands
    virtual void get(peer_connection& conn, const http_request& request);
    virtual void head(peer_connection& conn, const http_request& request);
    virtual void post(peer_connection& conn, const http_request& request);
    virtual void put(peer_connection& conn, const http_request& request);
    virtual void del(peer_connection& conn, const http_request& request);
    virtual void options(peer_connection& conn, const http_request& request);

    // request dispatcher
    void do_service(peer_connection& conn, const http_request& request);

    std::string get_error_code_str(int error_code); 

protected:
    
    // Helper function to build and send an HTTP successfull response
    void send_response(peer_connection& conn,
    	               std::string protocol, int error_code,
                       std::map< std::string, std::string > const & headers, 
                       std::string body="");

    // Helper function to build and send an HTTP error response
    void respond_error(peer_connection& conn, int error_code);
    
    std::string server_header() const {
        return "Server: Simple HTTP server 1.0";
    }
    
    // Default working directory (default: server's current directory)
    std::string files_dir;
};

#endif
