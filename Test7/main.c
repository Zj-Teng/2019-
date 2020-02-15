#include<stdio.h>
#include <stdbool.h>
#include<sys/types.h>
#include <winsock2.h>

#define ADDR "127.0.0.1"
#define PORT 8090
#define PORT_APACHE 8080
#define SIZE 1024

SOCKET serv;
SOCKET clnt;
SOCKET apac;
SOCKADDR_IN addrSrv;
SOCKADDR_IN addrCnt;
SOCKADDR_IN addrApa;

BOOL init();

DWORD WINAPI proxy(LPVOID para);

int main() {
    if (!init()) {
        printf("代理设置失败\n");
        return -1;
    }
    HANDLE proxyThread = CreateThread(NULL, 0, proxy, NULL, 0, NULL);
    WaitForSingleObject(proxyThread, INFINITE);
    return 0;
}

BOOL init() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        return false;
    }
    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
        WSACleanup();
        return false;
    }

    serv = socket(AF_INET, SOCK_STREAM, 0);
    apac = socket(AF_INET, SOCK_STREAM, 0);
    if (serv < 0 || apac < 0)
        return false;
    addrApa.sin_family = AF_INET;
    addrApa.sin_addr.s_addr = inet_addr(ADDR);
    addrApa.sin_port = htons(PORT_APACHE);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_addr.s_addr = inet_addr(ADDR);
    addrSrv.sin_port = htons(PORT);
    bind(serv, (struct sockaddr *) &addrSrv, sizeof(addrSrv));
    listen(serv, 5);
    return true;
}

DWORD WINAPI proxy(LPVOID para) {
    char clntBuff[SIZE]; //从客户端接收到的数据
    char servBuff[SIZE]; //从服务端接收到的数据
    int cntLen = sizeof(addrCnt);
//    while (true){
    clnt = accept(serv, (struct sockaddr *) &addrCnt, &cntLen);
    recv(clnt, clntBuff, SIZE, 0);
    printf("get from %s:%d-- %s\n", inet_ntoa(addrCnt.sin_addr),
           ntohs(addrCnt.sin_port), clntBuff);
    connect(apac, (struct sockaddr *) &addrApa, sizeof(addrApa));
    char text[SIZE];
    send(apac, clntBuff, SIZE, 0);
    recv(apac, servBuff, SIZE, 0);
    printf("servBuff: %s\n\n", servBuff);
    memset(servBuff, 0, SIZE);
    FILE *file = fopen("C:\\Users\\Administrator\\CLionProjects\\Test7\\Hello.html", "r");
    fread(text, SIZE, 1, file);
    sprintf(servBuff, "200 OK\r\nContent-Type: text/html\r\n\r\n%s\r\n\r\n", text);
    printf("%s\n", servBuff);
    send(clnt, servBuff, SIZE, 0);
    closesocket(clnt);
    closesocket(serv);
    WSACleanup();
//    }
    return 0L;
}