#include "framework.h"
#include "http_request.h"
#include "http_response.h"
#include "http_server.h"
#include "router.h"
#include "routes.h"

void Framework::run() {
    HttpServer server;

    Router router;
    setupRoutes(router);

    server.handleRequest = [&router](HttpRequest& request, HttpResponse& response) {
        router.handleRequest(request, response);
    };

    server.start();
}
