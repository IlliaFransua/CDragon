#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <unordered_map>

using std::string, std::to_string, std::unordered_map;

class HttpResponse {
public:
  // Setters
  void setStatus(int status);
  void addHeader(const string &key, const string &value);
  void addCookie(const string &key, const string &value);
  void setBody(const string &body);

  // Getters
  int getStatus() const noexcept { return status; }
  const unordered_map<string, string> &getHeaders() const noexcept {
    return headers;
  }
  const unordered_map<string, string> &getCookie() const noexcept {
    return cookie;
  }
  const string &getBody() const noexcept { return body; }
  string getResponse();

private:
  int status = 200;
  unordered_map<string, string> headers;
  unordered_map<string, string> cookie;
  string body;
};

// Setters
inline void HttpResponse::setStatus(int status) { this->status = status; }
inline void HttpResponse::addHeader(const string &key, const string &value) {
  headers.insert({key, value});
}
inline void HttpResponse::addCookie(const string &key, const string &value) {
  cookie.insert({key, value});
}
inline void HttpResponse::setBody(const string &body) {
  this->body = body;

  auto it = headers.find("Content-Length");
  if (it != headers.end()) {
    it->second = to_string(this->body.size());
  } else {
    this->addHeader("Content-Length", to_string(this->body.size()));
  }
}

#endif // HTTP_RESPONSE_H
