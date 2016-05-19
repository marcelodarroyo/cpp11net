#include "http_service.hpp"
#include <iostream>

void http_service::respond_error(peer_connection & conn, int error_code)
{
    std::map<std::string, std::string> headers;
    headers["Connection"] = "close";
    headers["Content-Type"] = "text/html; charset=UTF-8";
    std::string body = "<html><head></head><body><p>" +
                       get_error_code_str(error_code) +
                       "</p></body></html>";
    headers["Content-Length"] = std::to_string( body.length() );
    send_response(conn, "HTTP/1.1", error_code, headers, body);
}

void http_service::send_response(peer_connection & conn,
                                 std::string protocol, int error_code,
                                 std::map< std::string, std::string > const & headers,
                                 std::string body)
{
    std::string response = protocol + ' ' + get_error_code_str(error_code)  + END_LINE;
    response += server_header() + END_LINE; 
    for (auto h : headers)
        response += h.first + ": " + h.second + END_LINE;
    response += END_LINE + body;
    conn.send(response);
}

// Handle GET command
void http_service::get(peer_connection & conn, const http_request & request)
{
    std::string resource = request.path();

    if (resource == "/")
        resource = "index.html";

    std::cout << "Serving static file: " << files_dir + resource
              << " mime: " << request.mime() << std::endl;

    // get filesize
    std::ifstream f(files_dir + resource, std::ifstream::binary);
    if ( ! f.is_open() ) {
	std::cerr << "Error 1: Can't open file " << files_dir << resource << '\n';
        respond_error(conn, 404);
        return;
    }
    f.seekg(0, f.end);
    std::ifstream::pos_type file_size = f.tellg();
    f.close();

    // open for reading
    f.open(files_dir + resource, std::ifstream::binary);
    if ( ! f.is_open() ) {
	std::cerr << "Error 2: Can't open file " << files_dir << resource << '\n';
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

