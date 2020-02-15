#include <stdio.h>
#include <stdbool.h>
#include <Winsock2.h>

#define BUFFER_MAX 2048

SOCKET sockSrv;

DWORD WINAPI spanner(LPVOID lpParameter);
void init();

int main(int argc, char *argv[]){
    init();
    HANDLE thread1 = CreateThread(NULL, 0, spanner, NULL, 0, NULL);
    WaitForSingleObject(thread1, INFINITE);

    return 0;
}

void init(){
    //开始建立连接
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD( 1, 1 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        return ;
    }
    if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 ) {
        WSACleanup( );
        return ;
    }
    sockSrv=socket(AF_INET,SOCK_RAW,0);
    printf("sockSrv = %d\n", sockSrv);
    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    addrSrv.sin_family=AF_INET;
    addrSrv.sin_port=htons(6000);
    connect(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//连接建立完成
    bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
}

DWORD WINAPI spanner(LPVOID lpParameter){
    int n_read, proto;
    char buffer[BUFFER_MAX];
    char *ethhead, *iphead, *tcphead,*udphead, *icmphead, *p;
    if( sockSrv < 0){
        fprintf(stdout, "create socket error\n");
        exit(0);
    }
    while(true)
    {
        //成功则返回接收到的字符数, 失败则返回-1
        struct sockaddr_in from; //对端 socket 地址
        int iFromLen=sizeof(struct sockaddr_in); //地址结构大小
        int iReadDataLen; //接收数据长度
        n_read = recvfrom(sockSrv, buffer, BUFFER_MAX, 0, (struct sockaddr*)&from,&iFromLen);

/*
14 6(dest)+6(source)+2(type or length)
+
20 ip header
+
8 icmp,tcp or udp header
= 42
*/
        if(n_read < 42){
//            fprintf(stdout, "Incomplete header, packet corrupt\n");
            continue;
        }
        printf("收到%d字符\n", n_read);
        buffer[n_read] = '\0';
        if (strlen(buffer) != 0)
            printf("data: %s\n", buffer);
        ethhead = buffer;
        p = ethhead;
        int n = 0XFF;
        printf("MAC: %.2X:%02X:%02X:%02X:%02X:%02X==>"
               "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",
               p[6]&n, p[7]&n, p[8]&n, p[9]&n, p[10]&n, p[11]&n,
               p[0]&n, p[1]&n, p[2]&n,p[3]&n, p[4]&n, p[5]&n);
        iphead = ethhead + 14;
        p = iphead + 12;
        printf("IP: %d.%d.%d.%d => %d.%d.%d.%d\n",
               p[0]&0XFF, p[1]&0XFF, p[2]&0XFF, p[3]&0XFF,
               p[4]&0XFF, p[5]&0XFF, p[6]&0XFF, p[7]&0XFF);
        proto = (iphead + 9)[0];
        p = iphead + 20;
        printf("Protocol: ");
        switch(proto)
        {
            case IPPROTO_ICMP:  printf("ICMP\n");break;
            case IPPROTO_IGMP:  printf("IGMP\n");break;
            case IPPROTO_IP:    printf("IP\n");break;
            case IPPROTO_TCP :
            case IPPROTO_UDP :
                                printf("%s,", proto == IPPROTO_TCP ? "TCP": "UDP");
                                printf("source port: %u,",(p[0]<<8)&0XFF00 | p[1]&0XFF);
                                printf("dest port: %u\n", (p[2]<<8)&0XFF00 | p[3]&0XFF);
                                break;
            case IPPROTO_RAW :  printf("RAW\n");break;
            case IPPROTO_IDP :  printf("IDP\n");break;
            case IPPROTO_ND:    printf("ND\n");break;
            case IPPROTO_MAX:   printf("MAX\n");break;
            case IPPROTO_GGP:   printf("GGP\n");break;
            case IPPROTO_PUP:   printf("PUP\n");break;
//            default:printf("Unkown, please query in include/linux/in.h\n");
        }

        printf("\n\n");
    }
}