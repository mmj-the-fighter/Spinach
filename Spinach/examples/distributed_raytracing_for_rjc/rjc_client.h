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

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#define close_socket(s) closesocket(s)
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#define close_socket(s) close(s)
#endif

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
#ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
            int sock = (int)socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                std::cerr << "[rjc::HttpClient] socket creation failed\n";
#ifdef _WIN32
                WSACleanup();
#endif
                return response;
            }

#ifdef _WIN32
            DWORD timeoutMs = timeoutSec * 1000;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));
#else
            struct timeval tv;
            tv.tv_sec = timeoutSec;
            tv.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
#endif

            struct addrinfo hints;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            struct addrinfo* addrResult = nullptr;
            std::string portStr = std::to_string(port);
            int res = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &addrResult);
            if (res != 0 || addrResult == nullptr) {
                std::cerr << "[rjc::HttpClient] could not resolve host: " << host << "\n";
                close_socket(sock);
#ifdef _WIN32
                WSACleanup();
#endif
                return response;
            }

            int connRes = connect(sock, addrResult->ai_addr, (int)addrResult->ai_addrlen);
            freeaddrinfo(addrResult);

            if (connRes < 0) {
                std::cerr << "[rjc::HttpClient] connection to " << host << ":" << port << " failed\n";
                close_socket(sock);
#ifdef _WIN32
                WSACleanup();
#endif
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
            int sent = send(sock, reqStr.c_str(), (int)reqStr.size(), 0);
            if (sent < 0) {
                std::cerr << "[rjc::HttpClient] send failed\n";
                close_socket(sock);
#ifdef _WIN32
                WSACleanup();
#endif
                return response;
            }

            std::string rawResponse;
            char buffer[8192];
            int bytesRead = 0;
            while ((bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
                buffer[bytesRead] = '\0';
                rawResponse.append(buffer, bytesRead);
            }
            close_socket(sock);
#ifdef _WIN32
            WSACleanup();
#endif

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
