#ifndef ROUTER_H
#define ROUTER_H

#include "http_request.h"
#include "http_response.h"
#include <functional>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

using std::pair;
using std::regex;
using std::unordered_map, std::vector;

using handler_function = std::function<void(HttpRequest &, HttpResponse &)>;

class Router {
public:
  void addStaticRoute(const string &path, const handler_function &handler) {
    staticRoutes.insert({path, handler});
  }
  void addDynamicRoute(const string &path, const handler_function &handler) {
    //  '/company/<companyid>/user/<userid>'  is '/company/([^/]+)/user/([^/]+)'
    string regexPattern = regex_replace(path, regex("<[^/]+>"), "([^/]+)");
    dynamicRoutes.emplace_back(regex(regexPattern), handler);
  }

  void handleRequest(HttpRequest &, HttpResponse &);

private:
  unordered_map<string, handler_function> staticRoutes;
  vector<pair<regex, handler_function>> dynamicRoutes;

  bool handleStaticRoute(const string &path, HttpRequest &request,
                         HttpResponse &response);
  bool handleDynamicRoute(const string &path, HttpRequest &request,
                          HttpResponse &response);
};

#endif // ROUTER_H
