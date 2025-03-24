#include <gtest/gtest.h>
#include "router.h"
#include "http_request.h"
#include "http_response.h"


TEST(Router, StaticRouteMatch) {
    Router router;

    router.addStaticRoute("/static", [this](HttpRequest& req, HttpResponse& res) {
        res.setStatus(200);
        res.setBody("Static Route Matched");
    });

    string rawMetaData = "POST /static HTTP/1.1\r\n\
Host: localhost:8080\r\n\r\n";
    HttpRequest req;
    req.setRawMetaData(rawMetaData);
    req.parse();

    HttpResponse res;
    router.handleRequest(req, res);

    EXPECT_EQ(res.getStatus(), 200);
    EXPECT_EQ(res.getBody(), "Static Route Matched");
}


TEST(Router, DynamicRouteMatch) {
    Router router;

    router.addDynamicRoute("/user/<id>", [this](HttpRequest& req, HttpResponse& res) {
        res.setStatus(200);
        res.setBody("Dynamic Route Matched");
    });

    string rawMetaData = "POST /user/123 HTTP/1.1\r\n\
Host: localhost:8080\r\n\r\n";
    HttpRequest req;
    req.setRawMetaData(rawMetaData);
    req.parse();

    HttpResponse res;
    router.handleRequest(req, res);

    EXPECT_EQ(res.getStatus(), 200);
    EXPECT_EQ(res.getBody(), "Dynamic Route Matched");
}


TEST(Router, DynamicRouteWithManyParams) {
    Router router;

    router.addDynamicRoute("/company/<companyid>/user/<userid>", [this](HttpRequest& req, HttpResponse& res) {
        res.setStatus(200);
        res.setBody("Dynamic Route Matched");
    });

    string rawMetaData = "POST /company/456/user/789 HTTP/1.1\r\n\
Host: localhost:8080\r\n\r\n";
    HttpRequest req;
    req.setRawMetaData(rawMetaData);
    req.parse();

    HttpResponse res;
    router.handleRequest(req, res);

    EXPECT_EQ(res.getStatus(), 200);
    EXPECT_EQ(res.getBody(), "Dynamic Route Matched");
}


TEST(Router, RouteIsNotFound) {
    Router router;

    router.addDynamicRoute("/user/<id>", [this](HttpRequest& req, HttpResponse& res) {
        res.setStatus(200);
        res.setBody("Dynamic Route Matched");
    });

    string rawMetaData = "POST / HTTP/1.1\r\n\
Host: localhost:8080\r\n\r\n";
    HttpRequest req;
    req.setRawMetaData(rawMetaData);
    req.parse();

    HttpResponse res;
    router.handleRequest(req, res);

    EXPECT_EQ(res.getStatus(), 404);
    EXPECT_EQ(res.getBody(), "404 - Page is not found");
}
