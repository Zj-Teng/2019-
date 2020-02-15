#include <winsock2.h>
#include <stdio.h>
#include <stdbool.h>

#define MCASTADDR "233.0.0.1" //本例使用的多播组地址。
#define MCASTPORT 5150 //本地端口号。
#define BUFSIZE 1024 //发送数据缓冲大小。

SOCKET sock, sockM;
struct sockaddr_in local,remote,from;
int len;

void join();
DWORD WINAPI sendMes(LPVOID lpParameter);
DWORD WINAPI recvMes(LPVOID lpParameter);

int main(int argc, char **argv) {
    WSADATA wsd;
    len = sizeof(struct sockaddr_in);
    //初始化 WinSock2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
        printf("WSAStartup() failed\n");
        return -1;
    }
    join();
    HANDLE recvThread = CreateThread(NULL, 0, recvMes, NULL, 0, NULL);
    HANDLE sendThread = CreateThread(NULL, 0, sendMes, NULL, 0, NULL);
    WaitForSingleObject(recvThread, INFINITE);
    WaitForSingleObject(sendThread, INFINITE);
    return 0;
}

void join() {
    if((sock=WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,
                       WSA_FLAG_MULTIPOINT_C_LEAF|WSA_FLAG_MULTIPOINT_D_LEAF|
                       WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        printf("socket failed with:%d\n",WSAGetLastError());
        WSACleanup();
        return ;
    }
    //将 sock 绑定到本机某端口上。
    local.sin_family = AF_INET;
    local.sin_port = htons(MCASTPORT);
    local.sin_addr.s_addr = INADDR_ANY;
    if( bind(sock,(struct sockaddr*)&local,sizeof(local)) == SOCKET_ERROR )
    {
        printf( "bind failed with:%d \n",WSAGetLastError());
        closesocket(sock);WSACleanup();
        return ;
    }
    //加入多播组
    remote.sin_family = AF_INET;
    remote.sin_port = htons(MCASTPORT);
    remote.sin_addr.s_addr = inet_addr( MCASTADDR );
    /* Winsock1.0 */
    /*
    mcast.imr_multiaddr.s_addr = inet_addr(MCASTADDR);
    mcast.imr_interface.s_addr = INADDR_ANY;
    if( setsockopt(sockM,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcast,
    sizeof(mcast)) == SOCKET_ERROR)
    {
    printf("setsockopt(IP_ADD_MEMBERSHIP) failed:%d\n",WSAGetLastError());
    closesocket(sockM);
    WSACleanup();
    return -1;
    }
    */
    /* Winsock2.0*/
    if(( sockM = WSAJoinLeaf(sock,(SOCKADDR*)&remote,sizeof(remote),
                             NULL,NULL,NULL,NULL,
                             JL_BOTH)) == INVALID_SOCKET)
    {
        printf("WSAJoinLeaf() failed:%d\n",WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return ;
    }
}

DWORD WINAPI sendMes(LPVOID lpParameter) {
    TCHAR sendbuf[BUFSIZE];
    while(true)
    {
        printf("SEND : ");
        scanf("%s",sendbuf);
        if( sendto(sockM,(char*)sendbuf,strlen(sendbuf),0,(struct sockaddr*)
                &remote,sizeof(remote))==SOCKET_ERROR)
        {
            printf("sendto failed with: %d\n",WSAGetLastError());
            closesocket(sockM);
            closesocket(sock);
            WSACleanup();
            return -1;
        }
        if(strcmp(sendbuf,"QUIT")==0) return 0;
        Sleep(500);
    }
    closesocket(sockM);
    closesocket(sock);
    WSACleanup();
    return 0;
}

DWORD WINAPI recvMes(LPVOID lpParameter) {
    TCHAR recvbuf[BUFSIZE];
    int ret;
    while(true)
    {
        if(( ret = recvfrom(sock,recvbuf,BUFSIZE,0,
                            (struct sockaddr*)&from,&len)) == SOCKET_ERROR)
        {
            printf("recvfrom failed with:%d\n",WSAGetLastError());
            closesocket(sockM);
            closesocket(sock);
            WSACleanup();
            return -1;}
        if( strcmp(recvbuf,"quit") == 0 ) break;
        else {
            recvbuf[ret] = '\0';
            char addr[20];
            strcpy(addr, inet_ntoa(from.sin_addr));
//            printf("%s\n", addr);
            if (strcmp(addr, "10.1.11.198"))
                printf("RECV:' %s ' FROM <%s> \n",recvbuf,inet_ntoa(from.sin_addr));
        }
    }
    closesocket(sockM);
    closesocket(sock);
    WSACleanup();
    return 0;
}