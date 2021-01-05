#pragma once

#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS




#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../DataStructures/LinkedList.h";
#include "../DataStructures/ReadingStructure.h";

#pragma comment(lib,"WS2_32")




void SendToClient(SOCKET connectSocket, void* msg) {

    int iResult = send(connectSocket, (const char*)msg, sizeof(Measurment), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();

    }
    Sleep(10);
}