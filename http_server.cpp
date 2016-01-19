//=============================================================================
// Description: A simple C++ HTTP/1.1 server
// Author: Marcelo Arroyo (2014)
//=============================================================================

#include <chrono>
#include <ctime>
#include <string>
#include <ctype.h>	// for toupper()

#include "http_server.hpp"

#define END_LINE "\r\n"

std::string get_substring(const std::string & str, size_t & pos, 
                                        std::string end_mark)
{	
    size_t from = pos;
    std::string result;
    pos = str.find(end_mark, pos);
    if ( pos != std::string::npos )
        result = str.substr(from, pos - from);
    pos += end_mark.length();
    return result;
}

void http_request::parse_request(const std::string & request)
{
    _valid_request = true;
    size_t pos = 0;
    if ( parse_method(request, pos) )
        if ( parse_headers(request, pos) )
            _body = request.substr(pos);
}

bool http_request::parse_method(const std::string & request, size_t & pos)
{	
    _method = get_substring(request, pos, " ");
    for (auto & c : method)
        c = toupper(c);
    if ( method == "GET" || method == "HEAD" || method == "POST" || method == "PUT" || 
         method == "DELETE" || method == "TRACE" || method == "OPTIONS" ) {
        _resource = get_substring(request, pos, " ");
        _version = get_substring(request, pos, END_LINE);
        return true;
    }
    return false;
}

bool http_request::parse_headers(const std::string & request, size_t & pos)
{
    std::string line = get_substring(request, pos, END_LINE);
    while ( !line.empty() ) {
        size_t line_pos = 0; 
        std::string hdr = get_substring(line, line_pos, ": ");
        std::string val = line.substr(line_pos);
        _headers[hdr] = val;
        line = get_substring(request, pos, END_LINE);
    }
    return true;
}	

//=============================================================================
// http_service implementation
//=============================================================================
void http_service::do_service(peer_connection conn, const http_request & request)
{
    // default service
    std::string body = 
	    "<html>"
	    "<head>""
	    "<title>The Simplest HTTP server.</title>"
	    "</head>"
	    "<body>"
	    "<p>This is the simplest HTTP server.</p>"
	    "</body>"
	    "</html>";
	
    std::map<std::string, std::string> headers;
    headers["Server"] = "Simple HTTP server 1.0";
    headers["Content-Type"] = "text/html; charset=UTF-8";
    headers["Content-Length"] = std::to_string(body.length());
    headers["Connection"] = "close";
	
    send_response(conn, "HTTP/1.1", 200, headers, body);
}

std::string http_service::get_error_code_str(int error_code)
{
    std::string result;
    switch (error_code) {
        case 200: result = "200 OK"; break;
        case 400: result = "400 Bad Request"; break;
        case 500: result = "500 Internal Server Error"; break;
        case 501: result = "501 Not Implemented"; break;
    }
    return result;
}

void http_service::send_response(peer_connection conn,
                                 std::string protocol, int error_code,
				 std::map< std::string, std::string > const & headers,
				 std::string body)
{
    std::string response = protocol + ' ' + get_error_code_str(error_code)  + END_LINE;
    for (auto h : headers)
        response += h.first + ": " + h.second + END_LINE;
    response += END_LINE + body;
    conn.send(response);
}

//=============================================================================
// http_server class implementation
//=============================================================================
http_server::http_server(std::string node, std::string port)
{
    bind_to(AF_INET, node, port);
}

void http_server::register_service(std::string resource, http_service service)
{
    services[resource] = service;
}

void http_server::run()
{
    while( true ) {
        peer_connection c = accept_connection();
        http_request request(c.receive_string());
        dispatch_service(request, c);
    }
}

void http_server::dispatch_service(http_request const & request, peer_connection c)
{
    // TO DO: change to using thread_pool
    
    // search for a registered service
    auto service = services.find( request.get_resource() );
    if ( service != services.end() )
        // registered service found, dispatch it
        service->second.do_service(c,request);
    else {
        // dispatch default service
        http_service s;	
        s.do_service(c,request);
    }
}

// vim: set tabstop=4 shiftwidth=4 expandtab
