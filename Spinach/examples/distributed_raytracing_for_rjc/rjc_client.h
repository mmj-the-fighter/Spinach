#ifndef RJC_CLIENT_H
#define RJC_CLIENT_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdint>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace rjc
{
    struct HttpResponse {
        int statusCode = 0;
        std::string body;
    };

    class HttpClient {
    public:
        static HttpResponse PostJson(const std::string& host, int port, const std::string& path, const std::string& jsonBody, int timeoutSec = 120) {
            HttpResponse response;
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                std::cerr << "[rjc::HttpClient] socket creation failed\n";
                return response;
            }

            struct timeval tv;
            tv.tv_sec = timeoutSec;
            tv.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

            struct hostent* server = gethostbyname(host.c_str());
            if (server == nullptr) {
                std::cerr << "[rjc::HttpClient] could not resolve host: " << host << "\n";
                close(sock);
                return response;
            }

            struct sockaddr_in serv_addr;
            std::memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
            serv_addr.sin_port = htons(port);

            if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                std::cerr << "[rjc::HttpClient] connection to " << host << ":" << port << " failed\n";
                close(sock);
                return response;
            }

            std::ostringstream req;
            req << "POST " << path << " HTTP/1.1\r\n"
                << "Host: " << host << ":" << port << "\r\n"
                << "Content-Type: application/json\r\n"
                << "Content-Length: " << jsonBody.size() << "\r\n"
                << "Connection: close\r\n\r\n"
                << jsonBody;

            std::string reqStr = req.str();
            ssize_t sent = send(sock, reqStr.c_str(), reqStr.size(), 0);
            if (sent < 0) {
                std::cerr << "[rjc::HttpClient] send failed\n";
                close(sock);
                return response;
            }

            std::string rawResponse;
            char buffer[8192];
            ssize_t bytesRead = 0;
            while ((bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
                buffer[bytesRead] = '\0';
                rawResponse.append(buffer, bytesRead);
            }
            close(sock);

            size_t headerEnd = rawResponse.find("\r\n\r\n");
            if (headerEnd != std::string::npos) {
                std::string header = rawResponse.substr(0, headerEnd);
                response.body = rawResponse.substr(headerEnd + 4);

                size_t statusPos = header.find("HTTP/1.");
                if (statusPos != std::string::npos && statusPos + 9 <= header.size()) {
                    response.statusCode = std::atoi(header.substr(statusPos + 9, 3).c_str());
                }
            } else {
                response.body = rawResponse;
            }

            return response;
        }
    };
}

#endif
