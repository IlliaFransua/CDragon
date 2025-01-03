#include "http_server.h"
#include "http_request.h"
#include "http_response.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include <iomanip>
#include <limits>

using std::cout, std::endl, std::string;

void HttpServer::start() {
  int server_fd, new_connection;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Error creating a socket");
    exit(EXIT_FAILURE);
  }

  // binding socket to port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("Error configuring socket parameters");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET; // IP_v4
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Socket binding error");
    exit(EXIT_FAILURE);
  }

  // start listen
  if (listen(server_fd, 10) < 0) {
    perror("Listening error");
    exit(EXIT_FAILURE);
  }

  cout << "Server listening on port 8080..." << endl;

  while (true) {
    new_connection =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_connection < 0) {
      cout << "Failed to accept a new connection" << endl;
      continue;
    }

    char buffer[5000];
    string headers;
    string body;

    //      TODO: !!!! infinite cycle !!!!
    while (true) {
      int valread = read(new_connection, buffer, sizeof(buffer));
      if (valread <= 0) {
        break;
      }

      headers.append(string(buffer, valread));

      size_t header_end = headers.find("\r\n\r\n");
      if (header_end != string::npos) {
        break;
      }
    }

    if (headers.empty()) {
      cout << "Failed to read headers" << endl;
      continue;
    }

    size_t header_end = headers.find("\r\n\r\n");
    if (header_end == string::npos) {
      cout << "Failed to find the end of the headers" << endl;
      continue;
    } else {
      body = headers.substr(header_end + 4);
      headers = headers.substr(0, header_end + 4);

      size_t content_length_pos = headers.find("Content-Length:");
      if (content_length_pos != string::npos) {
        int content_length = std::stoi(headers.substr(content_length_pos + 15));

        body.reserve(content_length);
        int total_read = body.size();
        while (total_read < content_length) {
          //  TODO: wait time
          int valread = read(new_connection, buffer, sizeof(buffer));

          total_read += valread;
          body.append(string(buffer, valread));

          double progress = (double)total_read / content_length * 100.0;
          cout << "Loading progress: " << progress << "%" << endl;
        }
      }
    }

    HttpRequest request;
    cout << headers << endl;
    cout << body.substr(0, 1000) << endl;
    request.setRawMetaData(headers);
    request.setRawBody(body);
    request.parse();
    HttpResponse response;
    handleRequest(request, response);

    string responseStr = response.getResponse();
    send(new_connection, responseStr.c_str(), responseStr.size(), 0);

    close(new_connection);
  }

  close(server_fd);
  cout << "Server stopped" << endl;
}
