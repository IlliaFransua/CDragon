#include "http_request.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

using nlohmann::json;
using std::exception;
using std::string, std::istringstream, std::ostringstream, std::getline,
    std::cout, std::endl;

string trim(const string &str);

void HttpRequest::parse() {
  try {
    parseRawMetaData();
    parseRawBody();
  } catch (const exception &ex) {
    throw;
  }
}

void HttpRequest::parseRawMetaData() {
  if (rawMetaData.empty()) {
    throw std::runtime_error("Парсинг невозможен, так как HttpRequest получил "
                             "пустые мета даные запроса");
  }

  // first line
  istringstream stream(rawMetaData);
  getline(stream, method, ' ');
  getline(stream, path, ' ');
  getline(stream, protocolVersion, '\r');
  stream.ignore(); // skip '\n'

  // queryParams
  parseQueryParams();

  // headers
  parseHeaders(stream);
  parseCookies();
}

void HttpRequest::parseQueryParams() {
  string::size_type pos = path.find('?');
  if (pos != string::npos) {
    string queryString = path.substr(pos + 1);
    path = path.substr(0, pos);

    istringstream stream(queryString);
    string param;

    while (getline(stream, param, '&')) {
      string::size_type pos = param.find('=');
      if (pos != string::npos) {
        string key = param.substr(0, pos);
        string value = param.substr(pos + 1);

        queryParams.insert({key, value});
      }
    }
  }
}

void HttpRequest::parseHeaders(istringstream &stream) {
  string line;
  while (getline(stream, line)) {
    string::size_type pos = line.find(':');
    if (pos != string::npos) {
      string key = line.substr(0, pos);
      string value = line.substr(pos + 2);

      value = trim(value); // without [' ', '\r', '\n', '\t']

      headers.insert({key, value});
    }
  }
}

string trim(const string &str) {
  string::size_type first = str.find_first_not_of(" \r\n\t");
  if (first == string::npos)
    return ""; // str contains only [' ', '\r', '\n', '\t']
  string::size_type last = str.find_last_not_of(" \r\n\t");
  return str.substr(first, last - first + 1);
}

void HttpRequest::parseCookies() {
  unordered_map<string, string>::const_iterator pos = headers.find("Cookie");
  if (pos != headers.end()) {
    istringstream stream(pos->second);
    string part;

    while (getline(stream, part, ';')) {
      string::size_type pos = part.find('=');
      if (pos != string::npos) {
        string key = part.substr(0, pos);
        string value = part.substr(pos + 1);

        // could be better
        key = trim(key);
        value = trim(value);

        cookies.insert({key, value});
      }
    }

    headers.erase(pos);
  }
}

void HttpRequest::parseRawBody() {
  if (rawBody.empty()) {
    return;
  }

  auto pos = headers.find("Content-Type");
  if (pos != headers.end() && pos->second == "application/json") {
    parseJsonData();
  }
}

void HttpRequest::parseJsonData() {
  if (rawBody.empty()) {
    throw(std::runtime_error("body is empty"));
  }

  try {
    this->jsonData = json::parse(rawBody);
  } catch (const json::parse_error &e) {
    throw;
  } catch (const std::exception &e) {
    throw;
  }
}

void HttpRequest::parsePathParams(const std::smatch &match) {
  for (size_t i = 1; i < match.size(); ++i) {
    pathParams.emplace_back(match[i].str());
  }
}
