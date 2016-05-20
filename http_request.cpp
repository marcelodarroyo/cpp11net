//=======================================================================================
// Auxilary function for parsing request
//=======================================================================================

#include "http_request.hpp"

// files suffix - mime type binding
std::map< std::string, std::string > mime_types = {
    { ".txt",  "text/plain; charset=UTF-8" },
    { ".html", "text/html; charset=UTF-8" },
    { ".md",   "text/markdown" },
    { ".css",  "text/css" },
    { ".js",   "application/javascript" },
    { ".json", "application/json" },
    { ".xml",  "application/xml" },
    { ".png",  "image/png" },
    { ".oga",  "audio/ogg" },
    { ".ogv",  "video/ogg" },
    { ".pdf",  "application/pdf" }
};

std::string get_substring(const std::string & str, size_t & pos, std::string end_mark)
{	
    size_t from = pos;
    std::string result;
    pos = str.find(end_mark, pos);
    if ( pos != std::string::npos ) {
        result = str.substr(from, pos - from);
        pos += end_mark.length();
    }
    return result;
}

void http_request::parse_request(const std::string & request)
{
    size_t pos = 0;
    if ( parse_method(request, pos) )
        if ( parse_headers(request, pos) ) {
            _body = request.substr(pos);
            _valid_request = true;
        }
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
        if ( _version != "HTTP/1.0" && _version != "HTTP/1.1" )
            return false;
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
    auto it = mime_types.find( _url.substr( _url.find_last_of(".") )); 
    return it != mime_types.end() ? it->second : "application/octet-stream";
}

std::string http_request::path() const
{
    std::string::size_type begin_pos = _url.find("/"), 
                           end_pos = _url.find("?"),
                           count;
    // skip protocol (if exist)
    if ( begin_pos != std::string::npos && _url[begin_pos-1] == ':' )
        begin_pos = _url.find("/", begin_pos + 2 );
    count = (end_pos != std::string::npos)? end_pos - begin_pos : end_pos;
    return _url.substr(begin_pos, count);
}

// parse query with the form ...?q1=v1&q2=v2...
std::map<std::string,std::string> http_request::query() const
{
    std::map<std::string,std::string> query_map;
    std::string::size_type begin_pos = _url.find("?"), 
                           end_pos = _url.find("=");
    while ( begin_pos < _url.length() ) {
        std::string q = _url.substr(begin_pos + 1, end_pos);
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
    return ( it != _headers.end() )? it->second : "";
}
