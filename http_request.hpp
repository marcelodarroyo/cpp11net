#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <string>

// HTTP end line
#define END_LINE "\r\n"

//=============================================================================
// http request class: Represents a http request and contains parsing functions
// for getting request parts.
//=============================================================================
class http_request {
	
public:
	
    http_request() : _valid_request(false) { }
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

#endif
