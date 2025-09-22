#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in serverAddr, clientAddr;
    int port;
    char recvbuf[512];
    int clientAddrLen = sizeof(clientAddr);

    // Инициализация WinSock
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        std::cerr << "WSAStartup failed. Error code: " << wsaResult << std::endl;
        return 1;
    }

    std::cout << "Enter port for server: ";
    std::cin >> port;

    // Создание сокета
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Привязка к локальному адресу
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error code: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started. Waiting for messages on port " << port << "...\n";

    // Главный цикл: прием и отправка сообщений
    while (true) {
        int recvlen = recvfrom(sock, recvbuf, sizeof(recvbuf) - 1, 0, (sockaddr*)&clientAddr, &clientAddrLen);
        if (recvlen == SOCKET_ERROR) {
            std::cerr << "Error receiving data. Error code: " << WSAGetLastError() << std::endl;
            continue;
        }
        recvbuf[recvlen] = '\0';

        std::cout << "Received from client: " << recvbuf << std::endl;

        // Отправка обратно клиенту
        int sentlen = sendto(sock, recvbuf, recvlen, 0, (sockaddr*)&clientAddr, clientAddrLen);
        if (sentlen == SOCKET_ERROR) {
            std::cerr << "Error sending data. Error code: " << WSAGetLastError() << std::endl;
        }

        // Если строка начинается с 'x', завершить работу
        if (recvbuf[0] == 'x') break;
    }

    // Закрытие соединения
    closesocket(sock);
    WSACleanup();
    std::cout << "Server stopped." << std::endl;
    return 0;
}