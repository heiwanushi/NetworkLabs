// Серверная часть: TCP, "слушающий" сокет, accept(), getpeername()
// Русские комментарии сохранены

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    int result;

    // --- Инициализация WinSock ---
    result = WSAStartup(MAKEWORD(2, 0), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // --- Создание серверного сокета TCP ---
    SOCKET s_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_tcp == INVALID_SOCKET) {
        std::cout << "socket(...) for TCP error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // --- bind() для TCP ---
    sockaddr_in sa_tcp;
    unsigned short tcp_port = 2000;
    sa_tcp.sin_family = AF_INET;
    sa_tcp.sin_port = htons(tcp_port);
    sa_tcp.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(sa_tcp.sin_zero, 0, sizeof(sa_tcp.sin_zero));
    result = bind(s_tcp, (sockaddr*)&sa_tcp, sizeof(sa_tcp));
    if (result == SOCKET_ERROR) {
        std::cout << "bind(...) for TCP error: " << WSAGetLastError() << std::endl;
        closesocket(s_tcp);
        WSACleanup();
        return 1;
    }

    // --- listen() ---
    if (listen(s_tcp, 1) == SOCKET_ERROR) {
        std::cout << "listen(...) for TCP error: " << WSAGetLastError() << std::endl;
        closesocket(s_tcp);
        WSACleanup();
        return 1;
    }
    std::cout << "Server is listening on port " << tcp_port << std::endl;

    // --- accept() ---
    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    SOCKET s_client = accept(s_tcp, (sockaddr*)&client_addr, &client_addr_len);
    if (s_client == INVALID_SOCKET) {
        std::cout << "accept(...) for TCP error: " << WSAGetLastError() << std::endl;
        closesocket(s_tcp);
        WSACleanup();
        return 1;
    }
    std::cout << "Connection accepted from client." << std::endl;

    // --- Получение информации о клиенте ---
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    std::cout << "Client IP: " << client_ip << ", port: " << ntohs(client_addr.sin_port) << std::endl;

    // --- Получение информации о сервере через getpeername() ---
    sockaddr_in peer_addr;
    int peer_addr_len = sizeof(peer_addr);
    if (getpeername(s_client, (sockaddr*)&peer_addr, &peer_addr_len) == SOCKET_ERROR) {
        std::cout << "getpeername(...) for TCP error: " << WSAGetLastError() << std::endl;
    }
    else {
        char peer_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(peer_addr.sin_addr), peer_ip, INET_ADDRSTRLEN);
        std::cout << "Peer IP (from getpeername): " << peer_ip << ", port: " << ntohs(peer_addr.sin_port) << std::endl;
    }

    // --- Закрытие сокетов ---
    closesocket(s_client);
    closesocket(s_tcp);
    WSACleanup();
    std::cout << "Server finished." << std::endl;
    return 0;
}