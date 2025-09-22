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
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        std::cerr << "WSAStartup failed. Error code: " << wsaResult << std::endl;
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
        std::cerr << "Socket creation failed. Error code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    int inetRes = inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);
    if (inetRes <= 0) {
        std::cerr << "Invalid IP address format or address not supported.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Установка соединения
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed. Error code: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    std::cout << "Connection established!\n";

    std::cout << "Enter data to send: ";
    std::cin.getline(sendbuf, sizeof(sendbuf));
    size_t msglen = strlen(sendbuf);

    if (msglen == 0) {
        std::cerr << "No data entered to send. Exiting.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Шифруем данные
    std::vector<uint8_t> gamma = generate_gamma(key, msglen);
    std::vector<uint8_t> crypted(msglen);
    gamma_crypt((uint8_t*)sendbuf, crypted.data(), msglen, gamma);

    // Передача данных на сервер
    int sendResult = send(sock, (char*)crypted.data(), (int)msglen, 0);
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "Error sending data to server. Error code: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Прием ответа от сервера
    recvlen = recv(sock, recvbuf, sizeof(recvbuf), 0);
    if (recvlen == SOCKET_ERROR) {
        std::cerr << "Error receiving response from server. Error code: " << WSAGetLastError() << std::endl;
    }
    else if (recvlen == 0) {
        std::cout << "Server closed the connection.\n";
    }
    else {
        // Расшифровываем ответ
        std::vector<uint8_t> gammaR = generate_gamma(key, recvlen);
        std::vector<uint8_t> decrypted(recvlen);
        gamma_crypt((uint8_t*)recvbuf, decrypted.data(), recvlen, gammaR);
        std::cout << "Server response: ";
        std::cout.write((char*)decrypted.data(), recvlen);
        std::cout << std::endl;
    }

    // Закрытие соединения
    closesocket(sock);
    WSACleanup();
    return 0;
}