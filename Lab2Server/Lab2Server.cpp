#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "GammaCrypt.h"
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET listenSock = INVALID_SOCKET, clientSock = INVALID_SOCKET;
    sockaddr_in serverAddr, clientAddr;
    int port;
    char recvbuf[512], sendbuf[512];
    int clientAddrLen = sizeof(clientAddr);
    std::string key;

    std::cout << "Enter port for server: ";
    std::cin >> port;
    std::cin.ignore();
    std::cout << "Enter password (encryption key): ";
    std::getline(std::cin, key);

    // Инициализация WinSock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Создание сокета
    listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Привязка к локальному адресу
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Ожидание соединения
    listen(listenSock, SOMAXCONN);
    std::cout << "Waiting for client on port " << port << "...\n";
    clientSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
    std::cout << "Client connected!\n";

    // Прием данных от клиента
    int recvlen = recv(clientSock, recvbuf, sizeof(recvbuf), 0);
    if (recvlen > 0) {
        // Расшифровываем
        std::vector<uint8_t> gamma = generate_gamma(key, recvlen);
        std::vector<uint8_t> decrypted(recvlen);
        gamma_crypt((uint8_t*)recvbuf, decrypted.data(), recvlen, gamma);

        std::cout << "Received: ";
        std::cout.write((char*)decrypted.data(), recvlen);
        std::cout << std::endl;

        // Формируем ответ
        std::string reply = "Data received: ";
        reply.append((char*)decrypted.data(), recvlen);
        size_t replylen = reply.size();
        std::vector<uint8_t> gammaR = generate_gamma(key, replylen);
        std::vector<uint8_t> crypted(replylen);
        gamma_crypt((uint8_t*)reply.data(), crypted.data(), replylen, gammaR);

        // Отправка ответа клиенту
        send(clientSock, (char*)crypted.data(), (int)replylen, 0);
    }

    // Закрытие соединения
    closesocket(clientSock);
    closesocket(listenSock);
    WSACleanup();
    return 0;
}