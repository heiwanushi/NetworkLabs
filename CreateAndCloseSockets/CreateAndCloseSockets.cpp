// Программа для создания и закрытия сокетов TCP, IPX, SPX с выводом статуса работы каждого протокола
// Русские комментарии сохранены

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wsipx.h>  // Для IPX/SPX
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    int result;
    bool tcp_ok = false, ipx_ok = false, spx_ok = false;

    // --- Инициализация среды WinSock ---
    result = WSAStartup(MAKEWORD(2, 0), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup(...) error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // --- Создание сокета TCP ---
    SOCKET s_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_tcp == INVALID_SOCKET) {
        std::cout << "socket(...) for TCP error: " << WSAGetLastError() << std::endl;
    }
    else {
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
        }
        else {
            tcp_ok = true;
        }
    }

    // --- Создание сокета IPX ---
    SOCKET s_ipx = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);
    if (s_ipx == INVALID_SOCKET) {
        std::cout << "socket(...) for IPX error: " << WSAGetLastError() << std::endl;
    }
    else {
        // --- bind() для IPX ---
        sockaddr_ipx sa_ipx;
        unsigned short ipx_socketnum = 2000;
        memset(&sa_ipx, 0, sizeof(sa_ipx));
        sa_ipx.sa_family = AF_IPX;
        sa_ipx.sa_socket = ipx_socketnum;
        result = bind(s_ipx, (sockaddr*)&sa_ipx, sizeof(sa_ipx));
        if (result == SOCKET_ERROR) {
            std::cout << "bind(...) for IPX error: " << WSAGetLastError() << std::endl;
            closesocket(s_ipx);
        }
        else {
            ipx_ok = true;
        }
    }

    // --- Создание сокета SPX ---
    SOCKET s_spx = socket(AF_IPX, SOCK_STREAM, NSPROTO_SPX);
    if (s_spx == INVALID_SOCKET) {
        std::cout << "socket(...) for SPX error: " << WSAGetLastError() << std::endl;
    }
    else {
        // --- bind() для SPX ---
        sockaddr_ipx sa_spx;
        unsigned short spx_socketnum = 2000;
        memset(&sa_spx, 0, sizeof(sa_spx));
        sa_spx.sa_family = AF_IPX;
        sa_spx.sa_socket = spx_socketnum;
        result = bind(s_spx, (sockaddr*)&sa_spx, sizeof(sa_spx));
        if (result == SOCKET_ERROR) {
            std::cout << "bind(...) for SPX error: " << WSAGetLastError() << std::endl;
            closesocket(s_spx);
        }
        else {
            spx_ok = true;
        }
    }

    // --- Закрытие сокетов ---
    if (s_tcp != INVALID_SOCKET) closesocket(s_tcp);
    if (s_ipx != INVALID_SOCKET) closesocket(s_ipx);
    if (s_spx != INVALID_SOCKET) closesocket(s_spx);

    // --- Завершить работу с WinSock ---
    if (WSACleanup() == SOCKET_ERROR) {
        std::cout << "WSACleanup() error: " << WSAGetLastError() << std::endl;
    }

    // --- Итог проверки ---
    std::cout << std::endl << "*** Socket status ***" << std::endl;
    std::cout << "TCP: " << (tcp_ok ? "Works correctly" : "Does NOT work") << std::endl;
    std::cout << "IPX: " << (ipx_ok ? "Works correctly" : "Does NOT work") << std::endl;
    std::cout << "SPX: " << (spx_ok ? "Works correctly" : "Does NOT work") << std::endl;

    std::cout << "Program finished." << std::endl;
    return 0;
}