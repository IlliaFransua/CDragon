#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>

using nlohmann::json;
using std::string, std::vector, std::unordered_map, std::istringstream;

class HttpRequest {
public:
  // Setters of raw attributes
  void setRawMetaData(const string &rawMetaData) {
    this->rawMetaData = rawMetaData;
  }
  void setRawBody(const string &rawBody) { this->rawBody = rawBody; }

  // Parse Methods
  void parse();
  void parsePathParams(const std::smatch &match);

  // Getters
  const string &getMethod() const noexcept;
  const string &getPath() const noexcept;
  const string &getProtocolVersion() const noexcept;
  const unordered_map<string, string> &getQueryParams() const noexcept;
  const unordered_map<string, string> &getHeaders() const noexcept;
  const unordered_map<string, string> &getCookie() const noexcept;
  const string &getBody() const noexcept;
  const json &getJsonData() const noexcept;
  const vector<string> &getPathParams() const noexcept;

private:
  string rawMetaData;
  string rawBody;

  string method; // GET, POST etc
  string path;
  string protocolVersion;
  unordered_map<string, string> queryParams; // '/path?param=value'
  unordered_map<string, string> headers;
  unordered_map<string, string> cookies;
  json jsonData;
  vector<string> pathParams; // '/company/<companyid>/user/<userid>'

  // Parse Methods
  void parseRawMetaData();
  void parseQueryParams();
  void parseHeaders(istringstream &stream);
  void parseCookies();
  void parseRawBody();
  void parseJsonData();
};

// Getters
inline const string &HttpRequest::getMethod() const noexcept { return method; }
inline const string &HttpRequest::getPath() const noexcept { return path; }
inline const string &HttpRequest::getProtocolVersion() const noexcept {
  return protocolVersion;
}
inline const unordered_map<string, string> &
HttpRequest::getQueryParams() const noexcept {
  return queryParams;
}
inline const unordered_map<string, string> &
HttpRequest::getHeaders() const noexcept {
  return headers;
}
inline const unordered_map<string, string> &
HttpRequest::getCookie() const noexcept {
  return cookies;
}
inline const string &HttpRequest::getBody() const noexcept { return rawBody; }
inline const json &HttpRequest::getJsonData() const noexcept {
  return jsonData;
}
inline const vector<string> &HttpRequest::getPathParams() const noexcept {
  return pathParams;
}

#endif // HTTP_REQUEST_H
