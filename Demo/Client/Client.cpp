#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#pragma comment(lib,"WS2_32")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27016
#define ADDRESS "127.0.0.1"
#define TYPE_STRING_LENGHT 10
#define DEMOTESTCOUNT 20

int Init();
bool InitializeWindowsSockets();
bool CreateSocket();
bool Connect();
bool SendDemoMessage(int);

SOCKET connectSocket = INVALID_SOCKET;
sockaddr_in serverAddress;

typedef struct _msgFormat {
    char type[TYPE_STRING_LENGHT];
    int value;

}READING;

int main()
{
    int result = Init();
    if (result) {
        printf("ERROR CODE %d, press any key to exit\n", result);
        getchar();
        return result;
    }

    printf("Client initialised. Press enter for demo send\n");
    getchar();
    printf("Sending...\n");

    if (SendDemoMessage(DEMOTESTCOUNT)) {
        printf("Done, stopping..\n");
    }
    else {
        printf("An error occured\n");
    }
    getchar();

}

bool  SendDemoMessage(int times) {
    READING *msg = (READING*)malloc(sizeof(READING));
    msg->value = 13;

    for (int i = 0; i < times; ++i) {
        char type[10];
        itoa(i, type, 10); //itoa = int to ascii
        strcpy(msg->type, type);
        msg->value = i;

        int iResult = send(connectSocket, (const char*)msg, sizeof(READING), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return false;
        }
        Sleep(10);
    }
    return true;
}

int Init() {

    if (InitializeWindowsSockets() == false)
    {
        return 1;
    }
    if (CreateSocket() == false) {
        return 2;
    }
    if (Connect() == false) {
        return 3;
    }

    return 0;
}

bool Connect() {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(ADDRESS);
    serverAddress.sin_port = htons(DEFAULT_PORT);

    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
        return false;
    }
    return true;
}

bool CreateSocket() {
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return false;
    }
    return true;
}

bool InitializeWindowsSockets() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
    return true;
}