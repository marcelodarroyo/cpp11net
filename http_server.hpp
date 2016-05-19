//=============================================================================
// Description: A simple C++ HTTP/1.1 server
// Author: Marcelo Arroyo (2014)
//=============================================================================

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <map>
#include "stream_server.hpp"

//=============================================================================
// http request class: Represents a http request and contains parsing functions
// for getting request parts.
//=============================================================================
class http_request {
	
public:
	
    http_request() : _valid_request(false) {}
    http_request(const std::string & request) { parse_request(request); }

    std::string method() const { return _method; }
    std::string version() const { return _version; }
    std::string url() const { return _url; }
    std::map<std::string,std::string> headers() const { return _headers; }
    std::string header(std::string key) const;
    std::string body() const { return _body; }
    
    bool is_valid_request() const { return _valid_request; }

    std::string mime() const;
    std::string path() const;
    std::map<std::string,std::string> query() const;

protected:

    void parse_request(const std::string & request);
    bool parse_method(const std::string & request, size_t & pos);
    bool parse_headers(const std::string & request, size_t & pos);

private:

    std::string _method;
    std::string _url;
    std::string _version;
    std::map<std::string,std::string> _headers;
    std::string _body;
	
    bool _valid_request;
};

//=============================================================================
// An http service (base class). Default behaviour: serve static files with
// uploading and files listing support
//=============================================================================
class http_service {

public:
    http_service(std::string default_files_dir = ".")
        : files_dir(default_files_dir)
        {}

    virtual void do_service(peer_connection & conn, const http_request & request);

    std::string get_error_code_str(int error_code); 

protected:    
    // Special GET files-list request. Responds with a JSON list of file names.
    // It support a "suffix" query. 
    // Example: GET files-list?suffix=json (output is JSON of "ls *.json")
    void files_list(peer_connection & conn, const http_request & request);

    // Uploading files support with PUT HTTP command
    void handle_put(peer_connection & conn, const http_request & request);

    // Helper function to build and send an HTTP successfull response
    void send_response(peer_connection & conn,
    	               std::string protocol, int error_code,
                       std::map< std::string, std::string > const & headers, 
                       std::string body="");

    // Helper function to build and send an HTTP error response
    void respond_error(peer_connection & conn, int error_code);

    // Default working directory (default: server's current directory)
    std::string files_dir;
};

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
