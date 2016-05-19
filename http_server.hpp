//=============================================================================
// Description: A simple C++ HTTP/1.1 server
// Author: Marcelo Arroyo (2014)
//=============================================================================

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <map>
#include "stream_server.hpp"
#include "http_request.hpp"
#include "http_service.hpp"

//=============================================================================
// The http server class
//=============================================================================
class http_server : public stream_server {
public:
	
    // Bind to IP address (node) and service (port)
    http_server(std::string node, std::string port, std::string files_dir=".");
    void register_service(std::string resource, http_service service);
    void register_services_from_config(std::string config_file);

    // The service wrapper
    void service_fn(peer_connection & conn);

protected:

    // Service dispatcher
    void dispatch_service(http_request const & request, peer_connection & conn);
    
    // Registered services storage
    std::map<std::string, http_service> services;

    // Working directory
    std::string static_files_dir;
};

#endif

// vim: set tabstop=4 shiftwidth=4 expandtab:
