//=============================================================================
// Description: A simple C++ HTTP/1.1 server
// Author: Marcelo Arroyo (2014)
//=============================================================================

#include <chrono>
#include <ctime>
#include <string>
#include <ctype.h>	// for toupper(), isdigit(), ...
#include <fstream>
#include <iostream>

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
    for (auto & c : _method)
        c = toupper(c);
    // TO DO: Change to use a hash to check valid method
    if ( _method == "GET" || _method == "HEAD" || _method == "POST" || _method == "PUT" || 
         _method == "DELETE" || _method == "TRACE" || _method == "OPTIONS" ) {
        _url = get_substring(request, pos, " ");
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

std::string http_request::mime() const
{
    std::string mime_type = "application/octet-stream";
       
    if ( _url.find(".txt") != std::string::npos )
        mime_type = "text/plain; charset=UTF-8";
    else
    if ( _url.find(".html") != std::string::npos )
        mime_type = "text/html; charset=UTF-8";
    else
    if ( _url.find(".md") != std::string::npos )
        mime_type = "text/markdown";
    else 
    if ( _url.find(".css") != std::string::npos )
        mime_type = "text/css";
    else
    if ( _url.find(".js") != std::string::npos )
        mime_type = "application/javascript";
    else
    if ( _url.find(".xml") != std::string::npos )
        mime_type = "application/xml";
    else
    if ( _url.find(".png") != std::string::npos )
        mime_type = "image/png";
    else
    if ( _url.find(".oga") != std::string::npos )
        mime_type = "audio/ogg";
    else
    if ( _url.find(".ogv") != std::string::npos )
        mime_type = "video/ogg";
    else
    if ( _url.find(".pdf") != std::string::npos )
        mime_type = "application/pdf";

    return mime_type;
}

std::string http_request::path() const
{
    std::string::size_type 
        begin_pos = _url.find("/"), 
        end_pos = _url.find("?"),
        count;
    // skip protocol (if exist)
    if ( begin_pos > 0 && _url[begin_pos-1] == ':' )
        begin_pos = _url.find("/", begin_pos + 2 );
    count = (end_pos != std::string::npos)? end_pos - begin_pos : end_pos;
    return _url.substr(begin_pos, count);
}

// parse query with the form ...?q1=v1&q2=v2...
std::map<std::string,std::string> http_request::query() const
{
    std::map<std::string,std::string> query_map;
    std::string::size_type 
        begin_pos = _url.find("?"), 
        end_pos = _url.find("=");
    while ( begin_pos < _url.size() ) {
        std::string q = _url.substr(begin_pos+1, end_pos);
        std::string v;
        if ( end_pos != std::string::npos ) {
            begin_pos = end_pos + 1;
            end_pos = _url.find("&",begin_pos);
            v = _url.substr(begin_pos, end_pos);
            begin_pos = end_pos + 1;
        }
        query_map[q] = v;
    }
    return query_map;
}

std::string http_request::header(std::string key) const
{
    auto it = _headers.find(key); 
    return ( it != _headers.end() )? it->second : std::string();
}

//=============================================================================
// default http_service (static file service)
//=============================================================================
void http_service::do_service(peer_connection & conn, const http_request & request)
{
    std::string resource = request.path();
    std::string mime = request.mime();
    auto npos = std::string::npos;
    bool is_text_file = mime.find("text") != npos || 
	                mime.find("xml") != npos  ||
			mime.find("javascript") != npos;

    if (resource == "/")
        resource = "index.html";

    std::cout << "Serving static file: " << files_dir + resource
              << " Resource: " << resource << " Mime: " << mime << std::endl;

    // get filesize
    std::ifstream f(files_dir + resource, std::ifstream::ate);
    if ( ! f.is_open() ) {
        respond_error(conn, 404);
        return;
    }
    std::ifstream::pos_type file_size = f.tellg();
    f.close();

    // open for reading
    f.open( files_dir + resource, std::ifstream::in | is_text_file? 0: std::ifstream::binary);
    if ( ! f.is_open() ) {
        respond_error(conn, 404);
        return;
    }

    // build response
    std::map<std::string, std::string> headers;
    headers["Server"] = "Simple HTTP server 1.0";
    headers["Content-Type"] = "text/html; charset=UTF-8";
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

std::string http_service::get_error_code_str(int error_code)
{
    std::string result;
    switch (error_code) {
        case 200: result = "200 OK"; break;
        case 400: result = "400 Bad Request"; break;
        case 500: result = "500 Internal Server Error"; break;
        default : result = "501 Not Implemented"; break;
    }
    return result;
}

void http_service::respond_error(peer_connection & conn, int error_code)
{
    std::map<std::string, std::string> headers;
    headers["Server"] = "Simple HTTP server 1.0";
    headers["Connection"] = "close";
    headers["Content-Type"] = "text/html; charset=UTF-8";
    std::string body = "<html><head></head><body><p>" +
                       get_error_code_str(error_code) +
                       "</p></body></html>";
    headers["Content-Length"] = std::to_string(body.length());
    send_response(conn, "HTTP/1.1", error_code, headers, body);
}

void http_service::send_response(peer_connection & conn,
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
http_server::http_server(std::string node, std::string port, std::string files_dir)
    : static_files_dir(files_dir)
{
    bind_to(AF_INET, node, port);
}

void http_server::register_service(std::string resource, http_service service)
{
    services[resource] = service;
}

void http_server::register_services_from_config(std::string config_file)
{
    // TO DO
}

void http_server::run()
{
    while( true ) {
        peer_connection c = accept_connection();
        http_request request(c.receive_string());
        dispatch_service(request, c);
    }
}

void http_server::dispatch_service(http_request const & request, peer_connection & conn)
{
    // TO DO: use thread_pool
    
    std::string path = request.path();
    
    // REST support: see if path end with a number
    size_t last_slash_pos = path.find_last_of("/");
    bool ends_with_number = true;
    size_t pos = last_slash_pos;
    while ( ends_with_number && ++pos < path.length() )
        if ( ! isdigit(path[pos]) )
            ends_with_number = false; 
    if (ends_with_number)
      path.replace(last_slash_pos + 1, path.length() - last_slash_pos - 1, "{number}");
    
    auto service = services.find(path);
    if ( service != services.end() )
        // registered service found, dispatch it
        service->second.do_service(conn,request);
    else {
        // dispatch default service
        http_service s(static_files_dir);
        s.do_service(conn, request);
    }
}

// vim: set tabstop=4 shiftwidth=4 expandtab: 
