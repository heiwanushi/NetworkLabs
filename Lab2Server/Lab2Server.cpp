#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include "GammaCrypt.h"
#pragma comment(lib, "ws2_32.lib")

// Функция для обслуживания одного клиента
void handle_client(SOCKET clientSock, std::string key) {
    char recvbuf[512], sendbuf[512];
    int recvlen = recv(clientSock, recvbuf, sizeof(recvbuf), 0);
    if (recvlen == SOCKET_ERROR) {
        std::cerr << "Error receiving data from client. Error code: " << WSAGetLastError() << std::endl;
        closesocket(clientSock);
        return;
    }
    if (recvlen == 0) {
        std::cerr << "Client closed the connection before sending data." << std::endl;
        closesocket(clientSock);
        return;
    }

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
    int sendlen = send(clientSock, (char*)crypted.data(), (int)replylen, 0);
    if (sendlen == SOCKET_ERROR) {
        std::cerr << "Error sending response to client. Error code: " << WSAGetLastError() << std::endl;
    }

    // Закрытие соединения
    closesocket(clientSock);
    // После обслуживания клиента поток завершается
}

int main() {
    WSADATA wsaData;
    SOCKET listenSock = INVALID_SOCKET;
    sockaddr_in serverAddr, clientAddr;
    int port;
    int clientAddrLen = sizeof(clientAddr);
    std::string key;

    std::cout << "Enter port for server: ";
    std::cin >> port;
    std::cin.ignore();
    std::cout << "Enter password (encryption key): ";
    std::getline(std::cin, key);

    // Инициализация WinSock
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        std::cerr << "WSAStartup failed. Error code: " << wsaResult << std::endl;
        return 1;
    }

    // Создание сокета
    listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Привязка к локальному адресу
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error code: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    // Ожидание соединения
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed. Error code: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    std::cout << "Server started. Waiting for clients on port " << port << "...\n";

    // Главный цикл: сервер всегда слушает сеть
    while (true) {
        SOCKET clientSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSock == INVALID_SOCKET) {
            std::cerr << "Failed to accept client. Error code: " << WSAGetLastError() << std::endl;
            continue; // Сервер продолжает слушать
        }
        std::cout << "Client connected!\n";
        // Для каждого клиента создаем поток
        try {
            std::thread t(handle_client, clientSock, key);
            t.detach(); // Поток обслуживает клиента и завершается сам
        }
        catch (const std::exception& ex) {
            std::cerr << "Exception in client thread: " << ex.what() << std::endl;
            closesocket(clientSock);
        }
    }

    // Сервер не завершает работу самостоятельно, остановка — принудительно
    // Закрытие основного сокета — если потребуется
    // closesocket(listenSock);
    // WSACleanup();
    return 0;
}