#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "LinkedList.h"
#include "HashMap.h"
#include "ReadingStructure.h"

#pragma comment(lib,"WS2_32")

#define DEFAULT_BUFLEN 1000
#define DEFAULT_PORT "27016"
#define MAX_CLIENTS 10
#define TYPE_STRING_LENGHT 10
#define TIMEVAL_SEC 0
#define TIMEVAL_USEC 0
#define DEMOTESTCOUNT 20

bool InitializeWindowsSockets();
bool InitializeListenSocket();
bool BindListenSocket();
void SetNonBlocking();
int Init();
void Listen();
void SetAcceptedSocketsInvalid();
void ProcessMessages();
void SendToClient(SOCKET connectSocket, void* msg);





fd_set readfds;
SOCKET listenSocket = INVALID_SOCKET;
SOCKET acceptedSockets[MAX_CLIENTS];
addrinfo* resultingAddress = NULL;
timeval timeVal;
LIST* listHead = (LIST*)malloc(sizeof(LIST));
hashtable_t *hashMap = ht_create(DEFAULT_BUFLEN);

int received = 0;

int main()
{
    init_list(&listHead);
    int result = Init();
    if (result) {
        printf("ERROR CODE %d, press any key to exit\n", result);
        getchar();
        return result;
    }

    printf("Server live and ready to listen\n");
    SetAcceptedSocketsInvalid();
    Listen();
    getchar();
}

void SetAcceptedSocketsInvalid() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        acceptedSockets[i] = INVALID_SOCKET;
    }
}




void Listen() {


    int iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    printf("Listening...\n");

    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(listenSocket, &readfds);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (acceptedSockets[i] != INVALID_SOCKET)
                FD_SET(acceptedSockets[i], &readfds);
        }

        int value = select(0, &readfds, NULL, NULL, &timeVal);

        if (value == 0) {
            if (received == DEMOTESTCOUNT) {
                
                    
                   // test valid data

                    hash_elem_it it = HT_ITERATOR(hashMap);
                    hash_elem_t* e = ht_iterate(&it);

                    clock_t time_1 = clock();
                    while (e != NULL && e != nullptr)
                    {
                        printf("SENT: %s  \n", e->key);
                        SendToClient(acceptedSockets[0], e->data);
                        e = ht_iterate(&it);
                        
                    }
                    clock_t time_2 = clock();
                    printf("Hash table iterating n sending time: %d ms\n", (time_2 - time_1));
                    
                    clock_t time_3 = clock();
                    //LISTTraverseAndPrint(listHead);
                    while (listHead != NULL && listHead != nullptr) {
                        //  printf("Value %d: %d\n", ++i, head->value);
                        printf("SENT: %d  \n", listHead->value.value);
                        SendToClient(acceptedSockets[0], listHead);
                        listHead = listHead->next;
                    }
                    clock_t time_4 = clock();
                    printf("List iterating n sending time: %d ms\n", (time_4 - time_3));
                    printf("Server paused.Press any key to continue\n");
                    getchar();
                
                
                //TODO: izmeri vreme da se iterira kroz mapu i posalje sve nazad pa obrnuto
                
            }
        }
        else if (value == SOCKET_ERROR) {
            //Greska prilikom poziva funkcije
            printf("select failed with error: %d\n", WSAGetLastError());
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (FD_ISSET(acceptedSockets[i], &readfds)) {
                    closesocket(acceptedSockets[i]);
                    acceptedSockets[i] = INVALID_SOCKET;
                }
                else if (i == MAX_CLIENTS) {
                    closesocket(listenSocket);
                    WSACleanup();
                    return;
                }
            }
        }
        else {
            if (FD_ISSET(listenSocket, &readfds)) {   
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (acceptedSockets[i] == INVALID_SOCKET) {
                        acceptedSockets[i] = accept(listenSocket, NULL, NULL);
                        if (acceptedSockets[i] == INVALID_SOCKET)
                        {
                            printf("accept failed with error: %d\n", WSAGetLastError());
                            closesocket(listenSocket);
                            WSACleanup();
                            return;
                        }

                        break;
                    }
                }
                //ako mu ne nadje mesto uradi nesto

            }
            else { 
                ProcessMessages();
            }

        }
    } 
}

void ProcessMessages() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (FD_ISSET(acceptedSockets[i], &readfds)) {
            READING* recvbuf = (READING*)malloc(sizeof(READING));
            int iResult = recv(acceptedSockets[i], (char *)recvbuf, DEFAULT_BUFLEN, 0);
            if (iResult > 0)
            {
                printf("Message received from client: type: %s   val: %d count: %d \n", recvbuf->type, recvbuf->value, ++received);
                LISTInputElementAtStart(&listHead, *recvbuf);
                char* data = (char*)malloc(sizeof(char) * 10);
                itoa(recvbuf->value, data, 10);
                printf("%s\n", data);
                if (ht_put(hashMap, recvbuf->type, recvbuf) != NULL) {
                    printf("Error in ht_put\n");
                    getchar();
                }
            }
            else if (iResult == 0)
            {
                // connection was closed gracefully
                printf("Connection with client closed.\n");
                
                closesocket(acceptedSockets[i]);
                acceptedSockets[i] = INVALID_SOCKET;
            }
            else
            {
                // there was an error during recv
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(acceptedSockets[i]);
                acceptedSockets[i] = INVALID_SOCKET;
            }
        }
    }
}

int Init() {
    if (InitializeWindowsSockets() == false)
    {
        return 1;
    }


    if (InitializeListenSocket() == false) {
        return 2;
    }

    if (BindListenSocket() == false) {
        return 3;
    }

    freeaddrinfo(resultingAddress);

    SetNonBlocking();

    return 0;
}

void SetNonBlocking() {
    unsigned long mode = 1;

    int iResult = ioctlsocket(listenSocket, FIONBIO, &mode);

    FD_ZERO(&readfds);

    FD_SET(listenSocket, &readfds);

    timeVal.tv_sec = TIMEVAL_SEC;
    timeVal.tv_usec = TIMEVAL_USEC;
}

bool InitializeWindowsSockets()
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
    return true;
}

bool InitializeListenSocket() {
    addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; 
    hints.ai_flags = AI_PASSIVE;     

    int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return false;
    }

    return true;
}

bool BindListenSocket() {
    int iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }
    return true;
}

void SendToClient(SOCKET connectSocket, void * msg) {
   
        int iResult = send(connectSocket, (const char*)msg, sizeof(READING), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            
        }
        Sleep(10);
}