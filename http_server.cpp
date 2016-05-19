//=============================================================================
// Description: A simple C++ HTTP/1.1 server
// Author: Marcelo Arroyo (2014)
//=============================================================================

#include <chrono>
#include <ctime>
#include <string>
#include <ctype.h>	// for toupper(), isdigit(), ...
#include <iostream>

#include "http_server.hpp"

//=============================================================================
// http_server class implementation
//=============================================================================
http_server::http_server(std::string node, std::string port, std::string files_dir)
    : static_files_dir(files_dir)
{
    if ( ! bind_to(AF_INET, node, port) )
        std::cerr << "Bind error..." << std::endl;
}

void http_server::register_service(std::string resource, http_service service)
{
    services[resource] = service;
}

void http_server::register_services_from_config(std::string config_file)
{
    // TO DO
}

void http_server::dispatch_service(http_request const & request, peer_connection & conn)
{
    // TO DO: use thread_pool
    auto service = services.find( request.path() );
    if ( service != services.end() )
        // registered service found, dispatch it
        service->second.do_service(conn,request);
    else {
        // dispatch default file service
        http_service s(static_files_dir);
        s.do_service(conn, request);
    }
}

void http_server::service_fn(peer_connection & conn)
{
    http_request request = conn.receive_string();
#ifdef DEBUG
    std::cout << "Request from " << conn.get_peer_node()
              << ":" << conn.get_peer_port() << std::endl;
    std::cout << "Method: " << request.method() << std::endl
              << "Resource: " << request.url() << std::endl
              << "Protocol: " << request.version() << std::endl << std::endl;
    std::cout << "Headers: " << std::endl;
    for (auto & h : request.headers())
        std::cout << h.first << ": " << h.second << std::endl;
    std::cout << std::endl << "Body: " << std::endl;
    std::cout << request.body() << std::endl;
#endif
    dispatch_service(request, conn);
}

// vim: set tabstop=4 shiftwidth=4 expandtab: 
