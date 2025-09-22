#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "GammaCrypt.h"
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in serverAddr;
    std::string serverIp, key;
    int port;
    char sendbuf[512], recvbuf[512];
    int recvlen;

    // Инициализация WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    std::cout << "Enter server IP address: ";
    std::cin >> serverIp;
    std::cout << "Enter port: ";
    std::cin >> port;
    std::cin.ignore();
    std::cout << "Enter password (encryption key): ";
    std::getline(std::cin, key);

    // Создание сокета
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);

    // Установка соединения
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    std::cout << "Connection established!\n";

    std::cout << "Enter data to send: ";
    std::cin.getline(sendbuf, sizeof(sendbuf));
    size_t msglen = strlen(sendbuf);

    // Шифруем данные
    std::vector<uint8_t> gamma = generate_gamma(key, msglen);
    std::vector<uint8_t> crypted(msglen);
    gamma_crypt((uint8_t*)sendbuf, crypted.data(), msglen, gamma);

    // Передача данных на сервер
    send(sock, (char*)crypted.data(), (int)msglen, 0);

    // Прием ответа от сервера
    recvlen = recv(sock, recvbuf, sizeof(recvbuf), 0);
    if (recvlen > 0) {
        // Расшифровываем ответ
        std::vector<uint8_t> gammaR = generate_gamma(key, recvlen);
        std::vector<uint8_t> decrypted(recvlen);
        gamma_crypt((uint8_t*)recvbuf, decrypted.data(), recvlen, gammaR);
        std::cout << "Server response: ";
        std::cout.write((char*)decrypted.data(), recvlen);
        std::cout << std::endl;
    }
    else {
        std::cout << "No response from server.\n";
    }

    // Закрытие соединения
    closesocket(sock);
    WSACleanup();
    return 0;
}