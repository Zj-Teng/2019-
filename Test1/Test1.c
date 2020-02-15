#include <stdio.h>
#include <Winsock2.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    SOCKET conn;
} para;

DWORD WINAPI service(LPVOID lpParameter);
DWORD WINAPI client(LPVOID lpParameter);
DWORD WINAPI recvMes(LPVOID lpParameter);
DWORD WINAPI sendMes(LPVOID lpParameter);

int main() {
    HANDLE thread1;
    HANDLE thread2;
    thread1 = CreateThread(NULL, 0, service, NULL, 0, NULL);
    thread2 = CreateThread(NULL, 0, client, NULL, 0, NULL);
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);
}

DWORD WINAPI service(LPVOID lpParameter) {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        return NULL;
    }
    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
        WSACleanup();
        return NULL;
    }
    SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);
    bind(sockSrv, (SOCKADDR *) &addrSrv, sizeof(SOCKADDR));
    listen(sockSrv, 5);
    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);
    SOCKET sockConn = accept(sockSrv, (SOCKADDR *) &addrClient, &len);
    para *temp = (para *) malloc(sizeof(para));
    temp->conn = sockConn;
    HANDLE recvThread = CreateThread(NULL, 0, recvMes, temp, 0, NULL);
    HANDLE sendThread = CreateThread(NULL, 0, sendMes, temp, 0, NULL);
    WaitForSingleObject(recvThread, INFINITE);
    WaitForSingleObject(sendThread, INFINITE);
    closesocket(sockConn);
}

DWORD WINAPI client(LPVOID lpParameter) {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        return NULL;
    }
    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
        WSACleanup();
        return NULL;
    }
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);
    connect(sockClient, (SOCKADDR *) &addrSrv, sizeof(SOCKADDR));
    para *temp = (para *) malloc(sizeof(para));
    temp->conn = sockClient;
    HANDLE recvThread = CreateThread(NULL, 0, recvMes, temp, 0, NULL);
    HANDLE sendThread = CreateThread(NULL, 0, sendMes, temp, 0, NULL);
    WaitForSingleObject(recvThread, INFINITE);
    WaitForSingleObject(sendThread, INFINITE);
    closesocket(sockClient);    //关闭套接字
    WSACleanup(); //终止Winsock 2 DLL (Ws2_32.dll) 的使用
}

DWORD WINAPI recvMes(LPVOID lpParameter) {
    para *temp = (para *) lpParameter;
    while (true) {
        char recvBuf[50];
        printf("recv:");
        if (recv(temp->conn, recvBuf, 50, 0) != SOCKET_ERROR)
            printf("%s\n", recvBuf);
    }
}

DWORD WINAPI sendMes(LPVOID lpParameter){
    para *temp = (para *) lpParameter;
    while (true) {
        char sendBuf[50];
        printf("send:");
        scanf("%s", &sendBuf);
        send(temp->conn,sendBuf,strlen(sendBuf)+1,0);
    }
}