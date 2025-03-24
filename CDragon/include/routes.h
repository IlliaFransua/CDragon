#ifndef ROUTES_H
#define ROUTES_H

#include "router.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using std::string, std::ifstream, std::stringstream;

void setupRoutes(Router& router);

inline string loadFileContent(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

#endif // ROUTES_H
