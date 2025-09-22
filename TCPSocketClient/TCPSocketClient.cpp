// Клиентская часть: TCP, connect(), getpeername()
// Русские комментарии сохранены

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
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

    // --- Создание клиентского сокета TCP ---
    SOCKET s_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_tcp == INVALID_SOCKET) {
        std::cout << "socket(...) for TCP error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // --- bind() для клиента: порт 0 ---
    sockaddr_in sa_client;
    sa_client.sin_family = AF_INET;
    sa_client.sin_port = 0; // порт 0 для клиента
    sa_client.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(sa_client.sin_zero, 0, sizeof(sa_client.sin_zero));
    result = bind(s_tcp, (sockaddr*)&sa_client, sizeof(sa_client));
    if (result == SOCKET_ERROR) {
        std::cout << "bind(...) for client error: " << WSAGetLastError() << std::endl;
        closesocket(s_tcp);
        WSACleanup();
        return 1;
    }

    // --- connect() ---
    sockaddr_in sa_server;
    unsigned short tcp_port = 2000;
    std::string server_ip = "127.0.0.1"; // локальный сервер
    sa_server.sin_family = AF_INET;
    sa_server.sin_port = htons(tcp_port);
    // Исправлено: используем inet_pton вместо устаревшего inet_addr
    if (inet_pton(AF_INET, server_ip.c_str(), &sa_server.sin_addr) <= 0) {
        std::cout << "Invalid IP address format." << std::endl;
        closesocket(s_tcp);
        WSACleanup();
        return 1;
    }
    memset(sa_server.sin_zero, 0, sizeof(sa_server.sin_zero));
    result = connect(s_tcp, (sockaddr*)&sa_server, sizeof(sa_server));
    if (result == SOCKET_ERROR) {
        std::cout << "connect(...) for TCP error: " << WSAGetLastError() << std::endl;
        closesocket(s_tcp);
        WSACleanup();
        return 1;
    }
    std::cout << "Connected to server." << std::endl;

    // --- Получение информации о сервере через getpeername() ---
    sockaddr_in peer_addr;
    int peer_addr_len = sizeof(peer_addr);
    if (getpeername(s_tcp, (sockaddr*)&peer_addr, &peer_addr_len) == SOCKET_ERROR) {
        std::cout << "getpeername(...) for TCP error: " << WSAGetLastError() << std::endl;
    }
    else {
        char peer_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(peer_addr.sin_addr), peer_ip, INET_ADDRSTRLEN);
        std::cout << "Server IP (from getpeername): " << peer_ip << ", port: " << ntohs(peer_addr.sin_port) << std::endl;
    }

    // --- Получение информации о клиенте через getsockname() ---
    sockaddr_in local_addr;
    int local_addr_len = sizeof(local_addr);
    if (getsockname(s_tcp, (sockaddr*)&local_addr, &local_addr_len) == SOCKET_ERROR) {
        std::cout << "getsockname(...) error: " << WSAGetLastError() << std::endl;
    }
    else {
        char local_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(local_addr.sin_addr), local_ip, INET_ADDRSTRLEN);
        std::cout << "Local client IP: " << local_ip << ", port: " << ntohs(local_addr.sin_port) << std::endl;
    }

    // --- Закрытие сокета ---
    closesocket(s_tcp);
    WSACleanup();
    std::cout << "Client finished." << std::endl;
    return 0;
}