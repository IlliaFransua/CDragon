#include <gtest/gtest.h>
#include "http_response.h"
#include <string>
#include <map>
#include <iostream>

using std::string;
using std::cout, std::endl;

TEST(HttpResponse, All) {
    HttpResponse response;

    string rawMetaData = "HTTP/1.1 200\r\n\
Content-Length: 509\r\n\
X-Powered-By: MyApplicationFramework/2.3\r\n\
Server: MyCustomServer/1.0\r\n\
Cache-Control: no-cache\r\n\
Content-Type: application/json; charset=utf-8\r\n\
Pragma: no-cache\r\n\
Date: Wed, 31 Oct 2024 12:00:00 GMT\r\n\
Connection: keep-alive\r\n\
Access-Control-Allow-Origin: *\r\n\
X-XSS-Protection: 1; mode=block\r\n\
X-Content-Type-Options: nosniff\r\n\
Access-Control-Allow-Headers: Content-Type, Authorization\r\n\
Set-Cookie: Secure; SameSite=Strict; Path=/; session_id=abc123; HttpOnly\r\n\
\r\n";
    string rawBody = R"({"status": "success","data": {"user": {"id": 789,"name": "John Doe","email": "johndoe@example.com","roles": ["admin", "user"],"last_login": "2024-10-30T12:00:00Z"},"preferences": {"theme": "dark","language": "en-US","notifications": {"email": true,"sms": false}},"recent_activity": [{"action": "login","timestamp": "2024-10-30T12:00:00Z","ip_address": "192.168.1.1"},{"action": "update_profile","timestamp": "2024-10-31T10:00:00Z","ip_address": "192.168.1.2"}]},"message": "User data retrieved successfully."})";

    response.setStatus(200);
    std::map<string, string> headers = {
        {"Date", "Wed, 31 Oct 2024 12:00:00 GMT"},
        {"Server", "MyCustomServer/1.0"},
        {"Content-Type", "application/json; charset=utf-8"},
        {"Connection", "keep-alive"},
        // {"Set-Cookie", "session_id=abc123; Path=/; HttpOnly; Secure; SameSite=Strict"},
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"},
        {"Cache-Control", "no-cache"},
        {"Pragma", "no-cache"},
        {"X-Powered-By", "MyApplicationFramework/2.3"},
        {"X-Content-Type-Options", "nosniff"},
        {"X-XSS-Protection", "1; mode=block"}
    };

    for (const auto& header : headers) {
        response.addHeader(header.first, header.second);
    }
    
    response.addCookie("session_id", "abc123");
    response.addCookie("Path", "/");
    response.addCookie("Secure", "");
    response.addCookie("SameSite", "Strict");
    response.setBody(rawBody);

    
    EXPECT_EQ(response.getResponse(), rawMetaData + rawBody);
}