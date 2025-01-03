#include <gtest/gtest.h>
#include "http_request.h"
#include <string>
#include <iostream>

using std::string;
using std::cout, std::endl;

TEST(HttpRequest, WithNormalBody) {
    HttpRequest request;

    string rawMetaData = "POST /api/v1/users HTTP/1.1\r\n\
Host: localhost:8080\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Cookie: session_id=abc123; user_id=789; theme=dark\r\n\
Content-Length: 27\r\n";

    string rawBody = R"(username=johndoe&password=1234)";

    request.setRawMetaData(rawMetaData);
    request.setRawBody(rawBody);
    request.parse();
    
    EXPECT_EQ(request.getMethod(), "POST");
    EXPECT_EQ(request.getPath(), "/api/v1/users");
    EXPECT_EQ(request.getProtocolVersion(), "HTTP/1.1");
    EXPECT_EQ(request.getHeaders().at("Host"), "localhost:8080");
    EXPECT_EQ(request.getHeaders().at("Content-Type"), "application/x-www-form-urlencoded");
    EXPECT_EQ(request.getCookie().at("session_id"), "abc123");
    EXPECT_EQ(request.getCookie().at("user_id"), "789");
    EXPECT_EQ(request.getCookie().at("theme"), "dark");
    EXPECT_EQ(request.getHeaders().at("Content-Length"), "27");
    EXPECT_EQ(request.getBody(), "username=johndoe&password=1234");
}


TEST(HttpRequest, WithJsonBody) {
    HttpRequest request;

    string rawMetaData = "POST /api/v1/users HTTP/1.1\r\n\
Host: localhost:8080\r\n\
Content-Type: application/json\r\n\
Content-Length: 54\r\n";

    string rawBody = R"({"username":"johndoe","password":"1234"})";

    request.setRawMetaData(rawMetaData);
    request.setRawBody(rawBody);
    request.parse();

    EXPECT_EQ(request.getMethod(), "POST");
    EXPECT_EQ(request.getPath(), "/api/v1/users");
    EXPECT_EQ(request.getProtocolVersion(), "HTTP/1.1");
    EXPECT_EQ(request.getHeaders().at("Host"), "localhost:8080");
    EXPECT_EQ(request.getHeaders().at("Content-Type"), "application/json");
    EXPECT_EQ(request.getHeaders().at("Content-Length"), "54");
    EXPECT_EQ(request.getJsonData().at("username"), "johndoe");
    EXPECT_EQ(request.getJsonData().at("password"), "1234");
}


TEST(HttpRequest, WithOutBody) {
    HttpRequest request;

    string rawMetaData = "POST /api/v1/users HTTP/1.1\r\n\
Host: localhost:8080\r\n\
Content-Type: application/json\r\n";

    string rawBody;

    request.setRawMetaData(rawMetaData);
    request.setRawBody(rawBody);
    request.parse();

    EXPECT_EQ(request.getMethod(), "POST");
    EXPECT_EQ(request.getPath(), "/api/v1/users");
    EXPECT_EQ(request.getProtocolVersion(), "HTTP/1.1");
    EXPECT_EQ(request.getHeaders().at("Host"), "localhost:8080");
    EXPECT_EQ(request.getHeaders().at("Content-Type"), "application/json");
}
