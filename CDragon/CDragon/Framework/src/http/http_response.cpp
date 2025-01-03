#include "http_response.h"
#include <sstream>

using std::string;
using std::ostringstream;

string HttpResponse::getResponse() {
    ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    
    for(auto header : headers) {
        response << header.first << ": " << header.second << "\r\n";
    }

    if (!cookie.empty()) {
        response << "Set-Cookie: ";
        for(auto cook : cookie) {
            response << cook.first;
            if (!cook.second.empty()) {
                response << "=" << cook.second;
            }
            response << "; ";
        }
        response << "HttpOnly" << "\r\n";
    }

    response << "\r\n" << body;
    return response.str();
}
