#include "router.h"
#include "http_request.h"
#include <iostream>
#include <regex>

using std::regex, std::regex_replace, std::smatch;
using std::string;

void Router::handleRequest(HttpRequest &request, HttpResponse &response) {
  const string path = request.getPath();

  if (handleStaticRoute(path, request, response)) {
    return;
  }

  if (handleDynamicRoute(path, request, response)) {
    return;
  }

  response.setStatus(404);
  response.addHeader("Content-Type", "text/html; charset=utf-8");
  response.setBody("404 - Page is not found");
}

bool Router::handleStaticRoute(const string &path, HttpRequest &request,
                               HttpResponse &response) {
  auto it = staticRoutes.find(path);
  if (it != staticRoutes.end()) {
    it->second(request, response);
    return true;
  }
  return false;
}

bool Router::handleDynamicRoute(const string &path, HttpRequest &request,
                                HttpResponse &response) {
  for (const auto &[regexPattern, handler] : dynamicRoutes) {
    smatch match;
    if (regex_match(path, match, regexPattern)) {
      request.parsePathParams(match);
      handler(request, response);
      return true;
    }
  }
  return false;
}