#include "http_response.h"
#include <sstream>

using std::string, std::ostringstream;

string HttpResponse::getResponse() {
  ostringstream response;
  response << "HTTP/1.1 " << status << "\r\n";
  response << "Content-Length: " << body.size() << "\r\n";

  for (auto header : headers) {
    response << header.first << ": " << header.second << "\r\n";
  }

  response << "Set-Cookie: ";
  for (auto cook : cookie) {
    response << cook.first << "=" << cook.second << "; ";
  }
  response << "HttpOnly";

  response << "\r\n\r\n" << body;
  return response.str();
}
