#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <http_request.h>
#include <http_response.h>

using std::function;

class HttpServer {
public:
  void start();
  function<void(HttpRequest &, HttpResponse &)> handleRequest;
};

#endif // HTTP_SERVER_H
