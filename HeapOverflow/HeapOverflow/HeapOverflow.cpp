#define _CRT_SECURE_NO_DEPRECATE

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#pragma comment (lib, "Ws2_32.lib")

#define PORT "7777"
#define BUF_SIZE 4096

class Printer {
public:
    int ink;
    const char *ipAddie;
    Printer(const char *ip) {
        ipAddie = ip;
        ink = 100;
    }
    virtual void print() {
        printf("You have to pay $25 to get printed\n");
    }
};

void heckerlololololololololololo() {
    MessageBox(NULL, L"You aren't supposed to see this, if you are seeing this, it means you have been PWNED", L"PWNED", MB_OK);
}

//class Copier : public Printer {
//    
//
//};

struct ConnectionRequestArgies {//Unused, won't bother to remove
    const char* request;
    SOCKET clientSocket;
};

struct node {//Also unused, could remove
    char* data;
    struct node* next;
};
void theBestTargetForROP(int a) {
    printf("Fire Called");
    a++;//May be a useful ROP gadget
}

struct overwriteTarget {
    int a;//useless data
    void (*the_target)(int);
};

char* strings[30];//Make this global or smth

DWORD WINAPI connectionResponse(LPVOID cs) {
    printf("Called\n");
    SOCKET clientSocket = (SOCKET)cs;
    char inputBuffer[BUF_SIZE];
    char arg1[BUF_SIZE];
    char arg2[BUF_SIZE];
    int bytesRecieved;
    int number;
    int size;
    char* content;
    struct overwriteTarget* designatedForOverwrite = NULL;
    

    while (clientSocket) {
        bytesRecieved = recv(clientSocket, inputBuffer, BUF_SIZE, 0);
        int resultOfConn;
        if (bytesRecieved>0) {
            printf("%s\n", inputBuffer);
            int i = 0;
            if (strncmp(inputBuffer, "ALLOC", 5) == 0)//Unused Idea: Make a linked list
            {
                i = 0;
                send(clientSocket, "How much would you like to allocate?\n", sizeof("How much would you like to allocate?\n"), 0);
                resultOfConn = recv(clientSocket, arg1, BUF_SIZE, 0);//convert to int and return bad value if not
                size = atoi((const char*)arg1);
                while (strings[i] != NULL && i < 30) {
                    i++;
                }
                if (i >= 30) {
                    send(clientSocket, "Out of room for strings\n", sizeof("Out of room for strings\n"), 0);
                    continue;
                }
                if (size > 64 || size < 1) { //Make this unsigned
                    printf("Arg number %d\n", size);
                    send(clientSocket, "Buffer is too large\n", sizeof("Buffer is too large\n"), 0);
                    continue;
                }
                strings[i] = (char*)malloc(size);//I don't wanna free this
                //Replace with sprintf_s and then send
                send(clientSocket, "You have 64 bytes to send your message to the server. I hope you use them wisely\n", sizeof("You have 64 bytes to send your message to the server. I hope you use them wisely\n"), 0);
                printf("String Alloc'd at: %p\n", strings[i]);
                printf("Allocated: %d Bytes\n", size);
            }
            else if (strncmp(inputBuffer, "DEALLOC", 7) == 0) {
                send(clientSocket, "Where would you like to deallocate?\n", sizeof("Where would you like to deallocate?\n"), 0);
                resultOfConn = recv(clientSocket, arg1, BUF_SIZE, 0);//convert to int and return bad value if not
                number = atoi((const char*)arg1);
                if (strings[number]!=NULL) {
                    free(strings[number]);//Crashes here, find another way of checking nonexistence
                    send(clientSocket, "Successfully removed a string\n", sizeof("Successfully removed a string\n"), 0);
                }
                else {
                    send(clientSocket, "Invalid Index\n", sizeof("Invalid Index\n"), 0);
                    continue;
                }
            }
            else if (strncmp(inputBuffer, "INS", 3) == 0) {
                send(clientSocket, "Where would you like insert your text?\n", sizeof("Where would you like insert your text?\n"), 0);//Display a list of allocated buffers
                resultOfConn = recv(clientSocket, arg1, BUF_SIZE, 0);//convert to int and return bad value if not
                send(clientSocket, "What would you like to include in your buffer?\n", sizeof("What would you like to include in your buffer?\n"), 0);//Make sure the damn thing terminates
                resultOfConn = recv(clientSocket, arg2, BUF_SIZE, 0);//convert to int and return bad value if not

                number = atoi((const char*)arg1);
                content = arg2;

                strcpy(strings[(int)number], content); //Here's where all hell breaks loose
                send(clientSocket, "I hope you used your 64 bytes wisely\n", sizeof("I hope you used your 64 bytes wisely\n"), 0);//Change to be size of buffer
            }
            else if (strncmp(inputBuffer, "STRUCT", 6) == 0) {
                designatedForOverwrite = (struct overwriteTarget*)malloc(sizeof(struct overwriteTarget));//I should probably allocate the structs in an array
                if (designatedForOverwrite==NULL) {
                    send(clientSocket, "Structure Allocation Failed\n", sizeof("Structure Allocation Failed\n"), 0);
                    continue;
                }
                designatedForOverwrite->a = 4;
                designatedForOverwrite->the_target = &theBestTargetForROP;
                send(clientSocket, "Target has been created\n", sizeof("Target has been created\n"), 0);
                printf("Target Alloc'd at: %p\n", designatedForOverwrite);
            }
            else if (strncmp(inputBuffer, "FIRE", 4) == 0) {
                if(designatedForOverwrite!=NULL) {
                    (*designatedForOverwrite->the_target)(4);
                }
            }
            else if (strncmp(inputBuffer, "SHOW", 4) == 0) {
                send(clientSocket, "Which buffer would you like to show?\n> ", sizeof("Which buffer would you like to show?\n> "), 0);//Display a list of allocated buffers
                resultOfConn = recv(clientSocket, arg1, BUF_SIZE, 0);//convert to int and return bad value if not
                int indexToShow = atoi(arg1);
                if (strings[indexToShow] == NULL) {
                    break;
                }
                char* bufferToShow = strings[indexToShow];//Just copies the pointer on to the stack
                HANDLE hHeap = GetProcessHeap();
                SIZE_T newSize = HeapSize(hHeap, 0, strings[indexToShow]);
                printf("Size: %d", newSize);
                send(clientSocket, bufferToShow, newSize, 0);//I wasted 12 hours trying to intentionally make this leak
                
            }
            else {//Add a help command
                send(clientSocket, "Invalid Command\n", sizeof("Invalid Command\n"), 0);
            }
        }
    }
    return 0;
}

//Winsock Source: MS Documentation & Bing Co-Pilot
//The main putpose of this program is to practice remote heap-based buffer overflows, at the time I am writing this, learning winsock is not the main objective of this program
int main()
{
    WSADATA wsaData;
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;

    int socketStartUpResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (socketStartUpResult != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct addrinfo* result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, PORT, &hints, &result);
    bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (listenSocket == INVALID_SOCKET) {
        printf("\x07");
        printf("\033[31mListen failed with error: %d\033[0m\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("I got the server's ignition running :D\n");
    listen(listenSocket, SOMAXCONN);
    while (listenSocket) {
        printf("Listening for connections\n");
        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("\x07");
            printf("\033[31mAccept failed with error: %d\033[0m\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        printf("Recieved connection\n");
        send(clientSocket, "Welcome to the heap overflow server\n", sizeof("Welcome to the heap overflow server\n"), 0);
        
        //struct ConnectionRequestArgies connectionRequestArgies;
        //connectionRequestArgies.request = (char*)inputBuffer;
        CreateThread(0, 8192, (LPTHREAD_START_ROUTINE)connectionResponse, (LPVOID)clientSocket, 0, 0);//Only works for 1 connection, I'm sure somoene could DDOS this and cause a stack overflow
        printf("Finished with while\n");
    }
    closesocket(listenSocket);
    printf("Socket Closed\n");
    WSACleanup();
    return 0;
}