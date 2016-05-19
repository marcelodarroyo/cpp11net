#include "http_service.hpp"
#include <iostream>

// Handle GET command
void http_service::get(peer_connection & conn, const http_request & request)
{
    std::string resource = request.path();

    if (resource == "/")
        resource = "index.html";

    std::cout << "Serving static file: " << files_dir + resource
              << " Resource: " << resource << " mime: " << request.mime() << std::endl;

    // get filesize
    std::ifstream f(files_dir + resource, std::ifstream::ate);
    if ( ! f.is_open() ) {
        respond_error(conn, 404);
        return;
    }
    std::ifstream::pos_type file_size = f.tellg();
    f.close();

    // open for reading
    f.open( files_dir + resource, std::ifstream::in);
    if ( ! f.is_open() ) {
        respond_error(conn, 404);
        return;
    }

    // build response
    std::map<std::string, std::string> headers;
    headers["Server"] = "Simple HTTP server 1.0";
    headers["Content-Type"] = request.mime() + "; charset=UTF-8";
    headers["Content-Length"] = std::to_string(file_size);
    
    send_response(conn, "HTTP/1.1", 200, headers);

    // send body (file content)
    do {
        const unsigned int buf_size = 4096;
        char buffer[buf_size];
        f.read(buffer,buf_size);
        conn.send(buffer, f.gcount());
    } while ( f.gcount() > 0 );
    f.close();
}

// Handle POST command
void http_service::post(peer_connection & conn, const http_request & request)
{
    respond_error(conn, 501);
}

// PUT request: Create file with body request data
void http_service::put(peer_connection & conn, const http_request & request)
{
    std::cout << "PUT request: Creating " << files_dir << request.path() 
              << " file..." << std::endl;

    std::ofstream f(files_dir + request.path(), std::ofstream::out);
    f << request.body();
    f.close();

    // build response
    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "text/plain; charset=UTF-8";
    headers["Content-Length"] = "2";
    
    send_response(conn, "HTTP/1.1", 200, headers, "OK");
}

// Handle HEAD command
void http_service::head(peer_connection & conn, const http_request & request)
{
    respond_error(conn, 501);
}

// Handle DELETE command
void http_service::del(peer_connection & conn, const http_request & request)
{
    respond_error(conn, 501);
}

// Handle OPTIONS command
void http_service::options(peer_connection & conn, const http_request & request)
{
    respond_error(conn, 501);
}

