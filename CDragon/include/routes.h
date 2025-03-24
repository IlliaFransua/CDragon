#ifndef ROUTES_H
#define ROUTES_H

#include "router.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using std::string, std::ifstream, std::stringstream;

void setupRoutes(Router &router);

inline string loadFileContent(const string &filePath) {
  ifstream file(filePath);
  if (!file.is_open()) {
    return "";
  }

  stringstream buffer;
  buffer << file.rdbuf();

  file.close();
  return buffer.str();
}

#endif // ROUTES_H
