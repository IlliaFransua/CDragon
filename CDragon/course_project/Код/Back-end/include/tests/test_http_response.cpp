#include "http_response.h"
#include <gtest/gtest.h>

using namespace std;

TEST(HttpResponseTest, TestSetStatus) {
  HttpResponse response;
  response.setStatus(404);
  EXPECT_EQ(response.getStatus(), 404);
}

TEST(HttpResponseTest, TestAddHeader) {
  HttpResponse response;
  response.addHeader("Content-Type", "application/json");

  const auto &headers = response.getHeaders();
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at("Content-Type"), "application/json");
}

TEST(HttpResponseTest, TestAddMultipleHeaders) {
  HttpResponse response;
  response.addHeader("Content-Type", "application/json");
  response.addHeader("Server", "nginx");

  const auto &headers = response.getHeaders();
  EXPECT_EQ(headers.size(), 2);
  EXPECT_EQ(headers.at("Content-Type"), "application/json");
  EXPECT_EQ(headers.at("Server"), "nginx");
}

TEST(HttpResponseTest, TestAddCookie) {
  HttpResponse response;
  response.addCookie("session_id", "abc123");

  const auto &cookies = response.getCookie();
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at("session_id"), "abc123");
}

TEST(HttpResponseTest, TestSetBody) {
  HttpResponse response;
  string body = "This is the body of the response";
  response.setBody(body);

  EXPECT_EQ(response.getBody(), body);
}

TEST(HttpResponseTest, TestGetResponse) {
  HttpResponse response;
  response.setStatus(200);
  response.addHeader("Content-Type", "text/html");
  response.addCookie("session_id", "abc123");
  response.setBody(string("<html><body>Hello</body></html>"));

  string expectedResponse = "HTTP/1.1 200\r\n"
                            "Content-Length: 31\r\n"
                            "Content-Type: text/html\r\n"
                            "session_id: abc123\r\n"
                            "\r\n<html><body>Hello</body></html>";

  EXPECT_EQ(response.getResponse(), expectedResponse);
}

TEST(HttpResponseTest, TestGetResponseWithoutHeadersAndCookies) {
  HttpResponse response;
  response.setStatus(200);
  response.setBody("<html><body>Test</body></html>");

  string expectedResponse = "HTTP/1.1 200\r\n"
                            "Content-Length: 30\r\n"
                            "\r\n<html><body>Test</body></html>";

  EXPECT_EQ(response.getResponse(), expectedResponse);
}

TEST(HttpResponseTest, TestAddMultipleCookies) {
  HttpResponse response;
  response.addCookie("session_id", "abc123");
  response.addCookie("user_id", "456");

  const auto &cookies = response.getCookie();
  EXPECT_EQ(cookies.size(), 2);
  EXPECT_EQ(cookies.at("session_id"), "abc123");
  EXPECT_EQ(cookies.at("user_id"), "456");

  string expectedResponse = "HTTP/1.1 200\r\n"
                            "Content-Length: 0\r\n"
                            "user_id: 456\r\n"
                            "session_id: abc123\r\n"
                            "\r\n";
  EXPECT_EQ(response.getResponse(), expectedResponse);
}

TEST(HttpResponseTest, TestInvalidStatus) {
  HttpResponse response;
  response.setStatus(-1);
  EXPECT_EQ(response.getStatus(), -1);
}
