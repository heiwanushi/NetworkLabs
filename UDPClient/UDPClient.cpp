#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in clientAddr, serverAddr;
    std::string serverIp;
    int port;
    char sendbuf[512], recvbuf[512];
    int serverAddrLen = sizeof(serverAddr);

    // Инициализация WinSock
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        std::cerr << "WSAStartup failed. Error code: " << wsaResult << std::endl;
        return 1;
    }

    std::cout << "Enter server IP address: ";
    std::cin >> serverIp;
    std::cout << "Enter server port: ";
    std::cin >> port;
    std::cin.ignore();

    // Создание сокета
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Привязка для получения локального адреса
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    clientAddr.sin_port = 0; // Любой свободный порт
    if (bind(sock, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error code: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    int inetRes = inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);
    if (inetRes <= 0) {
        std::cerr << "Invalid server IP address format.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Главный цикл обмена данными
    while (true) {
        std::cout << "Enter string to send (max 511 chars): ";
        std::cin.getline(sendbuf, sizeof(sendbuf));
        size_t msglen = strlen(sendbuf);

        // Отправка данных на сервер
        int sendlen = sendto(sock, sendbuf, (int)msglen, 0, (sockaddr*)&serverAddr, serverAddrLen);
        if (sendlen == SOCKET_ERROR) {
            std::cerr << "Error sending data. Error code: " << WSAGetLastError() << std::endl;
            continue;
        }

        // Прием ответа от сервера
        int recvlen = recvfrom(sock, recvbuf, sizeof(recvbuf) - 1, 0, NULL, NULL);
        if (recvlen == SOCKET_ERROR) {
            std::cerr << "Error receiving data. Error code: " << WSAGetLastError() << std::endl;
            continue;
        }
        recvbuf[recvlen] = '\0';
        std::cout << "Received from server: " << recvbuf << std::endl;

        // Если строка начинается с 'x', завершить работу
        if (recvbuf[0] == 'x') break;
    }

    // Закрытие соединения
    closesocket(sock);
    WSACleanup();
    std::cout << "Client stopped." << std::endl;
    return 0;
}