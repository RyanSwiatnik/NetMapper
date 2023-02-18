// NetMapper.cpp : Defines the entry point for the application.
//

#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

using namespace std;

bool tcpPing(const char* host, const char* port) {
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return NULL;
    }

    // Create a TCP socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return NULL;
    }

    // Set the timeout for the socket
    int timeout = 1000; // milliseconds
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    // Resolve the host name
    addrinfo* result = NULL;
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    iResult = getaddrinfo(host, port, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }

    // Connect to the target
    iResult = connect(sock, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "connect failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(sock);
        WSACleanup();
        return false;
    }

    std::cout << "TCP ping to " << host << ":" << port << " succeeded" << std::endl;

    // Clean up
    freeaddrinfo(result);
    closesocket(sock);
    WSACleanup();

    return true;
}

int main() {
    
}