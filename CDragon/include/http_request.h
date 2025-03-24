#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>
#include <unordered_map>
#include <string_view>
#include <regex>

using std::string, std::vector, std::unordered_map, std::istringstream;

class HttpRequest {
public:
    // Setters of raw attributes
    void setRawMetaData(const string& rawMetaData) { this->rawMetaData = rawMetaData; }
    void setRawBody(const string& rawBody) { this->rawBody = rawBody; }

    // Parse Methods
    void parse();
    void parsePathParams(const std::smatch& match);

    // Getters
    const string& getMethod() const noexcept;
    const string& getPath() const noexcept;
    const string& getProtocolVersion() const noexcept;
    const unordered_map<string, string>& getQueryParams() const noexcept;
    const unordered_map<string, string>& getHeaders() const noexcept;
    const unordered_map<string, string>& getCookie() const noexcept;
    const string& getBody() const noexcept;
    const unordered_map<string, string>& getJsonData() const noexcept;
    const vector<string>& getPathParams() const noexcept;

private:
    string rawMetaData;
    string rawBody;

    string method;                        // GET, POST итд
    string path;                          // Путь запроса
    string protocolVersion;               // Версия протокола запроса
    unordered_map<string, string> queryParams;  // Query параметры (/path?param=value)
    unordered_map<string, string> headers;      // Заголовки запроса
    unordered_map<string, string> cookies;      // Куки
    unordered_map<string, string> jsonData;     // JSON данные из тела запроса
    vector<string> pathParams;            // Параметры пути (/company/<companyid>/user/<userid>)

    // Parse Methods
    void parseRawMetaData();
    void parseQueryParams();
    void parseHeaders(istringstream& stream);
    void parseCookies();
    void parseRawBody();
    void parseJsonData();
};

// Геттеры
inline const string& HttpRequest::getMethod() const noexcept { return method; }
inline const string& HttpRequest::getPath() const noexcept { return path; }
inline const string& HttpRequest::getProtocolVersion() const noexcept { return protocolVersion; }
inline const unordered_map<string, string>& HttpRequest::getQueryParams() const noexcept { return queryParams; }
inline const unordered_map<string, string>& HttpRequest::getHeaders() const noexcept { return headers; }
inline const unordered_map<string, string>& HttpRequest::getCookie() const noexcept { return cookies; }
inline const string& HttpRequest::getBody() const noexcept { return rawBody; }
inline const unordered_map<string, string>& HttpRequest::getJsonData() const noexcept { return jsonData; }
inline const vector<string>& HttpRequest::getPathParams() const noexcept { return pathParams; }

#endif // HTTP_REQUEST_H
