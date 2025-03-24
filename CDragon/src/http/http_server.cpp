#include "http_server.h"
#include "http_request.h"
#include "http_response.h"
#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iomanip>

using std::cout, std::endl, std::string;

void HttpServer::start() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error creating a socket");
        exit(EXIT_FAILURE);
    }

    // Binding a socket to port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Error configuring socket parameters");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Socket binding error");
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, 3) < 0) {
        perror("Listening error");
        exit(EXIT_FAILURE);
    }

    cout << "Server listening on port 8080..." << endl;

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            cout << "Failed to accept a new connection" << endl;
            continue;
        }

        char buffer[10000];
        string headers;
        string body;

        // Reading data from socket
        //      TODO: !!!! infinite cycle !!!!
        while (true) {
            int valread = read(new_socket, buffer, sizeof(buffer));
            if (valread <= 0) {
                break;
            }

            headers.append(string(buffer, valread));

            size_t header_end = headers.find("\r\n\r\n");
            if (header_end != string::npos) {
                // headers = headers.substr(0, header_end + 4);
                break;
            }
        }

        if (headers.empty()) {
            cout << "Failed to read headers" << endl;
            continue;
        }

        // cout << "Headers:\n" << headers << endl;

        size_t header_end = headers.find("\r\n\r\n");
        if (header_end != string::npos) {
            body = headers.substr(header_end + 4);

            size_t content_length_pos = headers.find("Content-Length:");
            if (content_length_pos != string::npos) {
                int content_length = std::stoi(headers.substr(content_length_pos + 15));

                // cout << "Ожидаемое тело: " << content_length << " байт" << endl;

                body.reserve(content_length);
                int total_read = body.size();
                while (total_read < content_length) {
                    int valread = read(new_socket, buffer, sizeof(buffer));
                    
                    // if (valread <= 0) {
                    //     break;
                    // }

                    total_read += valread;
                    body.append(string(buffer, valread));

                    double progress = (double)total_read / content_length * 100.0;
                    cout << "Loading progress: " << std::fixed << std::setprecision(2) << progress << "%" << std::flush;
                }
            }

            // cout << "Body:\n" << body << endl;
        } else {
            cout << "Failed to find the end of the headers" << endl;
            continue;
        }

        HttpRequest request;
        request.setRawMetaData(headers);
        request.setRawBody(body);
        request.parse();
        HttpResponse response;
        handleRequest(request, response);
        
        string responseStr = response.getResponse();
        send(new_socket, responseStr.c_str(), responseStr.size(), 0);

        close(new_socket);
    }

    close(server_fd);
    cout << "Server stopped" << endl;
}
