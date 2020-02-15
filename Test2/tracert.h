//
// Created by Administrator on 2019/12/11.
//

#ifndef TEST2_TRACERT_H
#define TEST2_TRACERT_H

#endif //TEST2_TRACERT_H
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdbool.h>

typedef struct {
    unsigned char hdr_len:4; //4 位头部长度
    unsigned char version:4; //4 位版本号
    unsigned char tos; //8 位服务类型
    unsigned short total_len; //16 位总长度
    unsigned short identifier; //16 位标识符
    unsigned short frag_and_flags; //3 位标志加 13 位片偏移
    unsigned char ttl; //8 位生存时间
    unsigned char protocol; //8 位上层协议号
    unsigned short checksum; //16 位校验和
    unsigned long sourceIP; //32 位源 IP 地址
    unsigned long destIP; //32 位目的 IP 地址
} IP_HEADER;

typedef struct {
    short sin_family;
    u_short sin_port;
    struct in_addr sin_addr;
    char sin_sero[8];
} sockaddr_in;

//ICMP 报头
typedef struct {
    BYTE type; //8 位类型字段
    BYTE code; //8 位代码字段
    USHORT cksum; //16 位校验和
    USHORT id; //16 位标识符
    USHORT seq; //16 位序列号
} ICMP_HEADER;

//报文解码结构
typedef struct {
    USHORT usSeqNo; //序列号
    DWORD dwRoundTripTime; //往返时间
    struct in_addr dwIPaddr; //返回报文的 IP 地址
} DECODE_RESULT;

const int MAX_ICMP_PACKET_SIZE=1024;//ICMP报文最大长度（包括报头）
const int DEF_ICMP_DATA_SIZE=32;//ICMP报文默认数据字段长度
const int ICMP_ECHO_REQUEST=8;//ICMP类型字段，8表示请求回显
const BYTE ICMP_ECHO_REPLY = 0; //回显应答
const BYTE ICMP_TIMEOUT = 11; //传输超时
const int DEF_MAX_HOP = 30; //最大跳站数
const DWORD DEF_ICMP_TIMEOUT = 3000; //回显应答超时时间

USHORT checksum(USHORT *pBuf, int iSize);
BOOL DecodeIcmpResponse(char *pBuf, int iPacketSize, DECODE_RESULT *DecodeResult, BYTE ICMP_ECHO_REPLY, BYTE ICMP_TIMEOUT);
BOOL tracert(char *IpAddress);
BOOL isValid(u_long* ulDestIP, char *IpAddress);

//计算网际校验和函数
USHORT checksum(USHORT *pBuf, int iSize) {
    unsigned long cksum = 0;
    while (iSize > 1) {
        cksum += *pBuf++;
        iSize -= sizeof(USHORT);
    }
    if (iSize) {
        cksum += *(UCHAR *) pBuf;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (USHORT) (~cksum);
}

//对数据包进行解码
BOOL DecodeIcmpResponse(char *pBuf, int iPacketSize, DECODE_RESULT *DecodeResult, BYTE ICMP_ECHO_REPLY, BYTE ICMP_TIMEOUT) {
    //检查数据报大小的合法性
    IP_HEADER *pIpHdr = (IP_HEADER *) pBuf;
    int iIpHdrLen = pIpHdr->hdr_len * 4;
    if (iPacketSize < (int) (iIpHdrLen + sizeof(ICMP_HEADER)))
        return FALSE;
    //根据 ICMP 报文类型提取 ID 字段和序列号字段
    ICMP_HEADER *pIcmpHdr = (ICMP_HEADER *) (pBuf + iIpHdrLen);
    USHORT usID, usSquNo;
    if (pIcmpHdr->type == ICMP_ECHO_REPLY) //ICMP 回显应答报文
    {
        usID = pIcmpHdr->id; //报文 ID
        usSquNo = pIcmpHdr->seq; //报文序列号
    } else if (pIcmpHdr->type == ICMP_TIMEOUT)//ICMP 超时差错报文
    {
        char *pInnerIpHdr = pBuf + iIpHdrLen + sizeof(ICMP_HEADER); //载荷中的 IP 头
        int iInnerIPHdrLen = ((IP_HEADER *) pInnerIpHdr)->hdr_len * 4; //载荷中的 IP 头长
        ICMP_HEADER *pInnerIcmpHdr = (ICMP_HEADER *) (pInnerIpHdr + iInnerIPHdrLen);//载荷中的 ICMP 头
        usID = pInnerIcmpHdr->id; //报文 ID
        usSquNo = pInnerIcmpHdr->seq; //序列号
    } else {
        return false;
    }
    //检查 ID 和序列号以确定收到期待数据报
    if (usID != (USHORT) GetCurrentProcessId() || usSquNo != DecodeResult->usSeqNo) {
        return false;
    }
    //记录 IP 地址并计算往返时间
    DecodeResult->dwIPaddr.s_addr = pIpHdr->sourceIP;
    DecodeResult->dwRoundTripTime = GetTickCount() - DecodeResult->dwRoundTripTime;
    //处理正确收到的 ICMP 数据报
    if (pIcmpHdr->type == ICMP_ECHO_REPLY || pIcmpHdr->type == ICMP_TIMEOUT) {
        //输出往返时间信息
        if (DecodeResult->dwRoundTripTime) {
            printf("%dms", DecodeResult->dwRoundTripTime);
        } else {
            printf("<1ms");
        }
    }
    return true;
}

BOOL isValid(u_long* ulDestIP, char *IpAddress) {
    //转换不成功时按域名解析
    if (*ulDestIP == INADDR_NONE) {
        struct hostent *pHostent = gethostbyname(IpAddress);
        if (pHostent) {
            *ulDestIP = (*(struct in_addr *) pHostent->h_addr).s_addr;
            return true;
        } else {
            printf("输入的 IP 地址或域名无效!\n");
            WSACleanup();
            return false;
        }
    }
}

BOOL tracert(char *IpAddress) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
//    cout << "请输入一个 IP 地址或域名： ";
//    cin >> IpAddress;
    //得到 IP 地址
    u_long ulDestIP = inet_addr(IpAddress);
    if (!isValid(&ulDestIP, IpAddress)){
        return false;
    }
//    cout << "Tracing route to " << IpAddress << " with a maximum of 30 hops.\n" << endl;
    //填充目地端 socket 地址
    sockaddr_in destSockAddr;
    ZeroMemory(&destSockAddr, sizeof(sockaddr_in));
    destSockAddr.sin_family = AF_INET;
    destSockAddr.sin_addr.s_addr = ulDestIP;
    //创建原始套接字
    SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0,
                               WSA_FLAG_OVERLAPPED);
    //超时时间
    int iTimeout = 500;
    //接收超时
    setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
    //发送超时
    setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
    //构造 ICMP 回显请求消息，并以 TTL 递增的顺序发送报文
    //填充 ICMP 报文中每次发送时不变的字段
    char IcmpSendBuf[sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE];//发送缓冲区
    memset(IcmpSendBuf, 0, sizeof(IcmpSendBuf)); //初始化发送缓冲区
    char IcmpRecvBuf[MAX_ICMP_PACKET_SIZE]; //接收缓冲区
    memset(IcmpRecvBuf, 0, sizeof(IcmpRecvBuf)); //初始化接收缓冲区
    ICMP_HEADER *pIcmpHeader = (ICMP_HEADER *) IcmpSendBuf;
    pIcmpHeader->type = ICMP_ECHO_REQUEST; //类型为请求回显
    pIcmpHeader->code = 0; //代码字段为 0
    pIcmpHeader->id = (USHORT) GetCurrentProcessId(); //ID 字段为当前进程号
    memset(IcmpSendBuf + sizeof(ICMP_HEADER), 'E', DEF_ICMP_DATA_SIZE);//数据字段
    USHORT usSeqNo = 0; //ICMP 报文序列号
    int iTTL = 1; //TTL 初始值为 1
    BOOL bReachDestHost = FALSE; //循环退出标志
    int iMaxHot = DEF_MAX_HOP; //循环的最大次数
    DECODE_RESULT DecodeResult; //传递给报文解码函数的结构化参数
    while (!bReachDestHost && iMaxHot--) {
        //设置 IP 报头的 TTL 字段
        setsockopt(sockRaw, IPPROTO_IP, IP_TTL, (char *) &iTTL, sizeof(iTTL));
        //printf("%d", iTTL);
//        cout << iTTL << flush; //输出当前序号
        //填充 ICMP 报文中每次发送变化的字段
        ((ICMP_HEADER *) IcmpSendBuf)->cksum = 0; //校验和先置为 0
        ((ICMP_HEADER *) IcmpSendBuf)->seq = htons(usSeqNo++); //填充序列号
        ((ICMP_HEADER *) IcmpSendBuf)->cksum = checksum((USHORT *) IcmpSendBuf,
                                                        sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE); //计算校验和
        //记录序列号和当前时间
        DecodeResult.usSeqNo = ((ICMP_HEADER *) IcmpSendBuf)->seq; //当前序号
        DecodeResult.dwRoundTripTime = GetTickCount(); //当前时间
        //发送 TCP 回显请求信息
        sendto(sockRaw, IcmpSendBuf, sizeof(IcmpSendBuf), 0, (struct sockaddr *) &destSockAddr, sizeof(destSockAddr));
        //接收 ICMP 差错报文并进行解析处理
        sockaddr_in from; //对端 socket 地址int iFromLen=sizeof(from); //地址结构大小
        int iFromLen=sizeof(from); //地址结构大小
        int iReadDataLen; //接收数据长度
        while (1) {
            //接收数据
            iReadDataLen = recvfrom(sockRaw, IcmpRecvBuf, MAX_ICMP_PACKET_SIZE, 0, (struct sockaddr *) &from, &iFromLen);
            if (iReadDataLen != SOCKET_ERROR)//有数据到达
            {
                //对数据包进行解码
                if (DecodeIcmpResponse(IcmpRecvBuf, iReadDataLen, &DecodeResult, ICMP_ECHO_REPLY, ICMP_TIMEOUT)) {
                    //到达目的地，退出循环
                    if (DecodeResult.dwIPaddr.s_addr == destSockAddr.sin_addr.s_addr)
                        bReachDestHost = true;
                    //输出 IP 地址
                    //printf("%20s\n", inet_ntoa(DecodeResult.dwIPaddr));
                    return true;
//                    cout << '\t' << inet_ntoa(DecodeResult.dwIPaddr) << endl;
//                    break;
                }
            } else if (WSAGetLastError() == WSAETIMEDOUT) //接收超时，输出*号
            {
                //printf(" *\tRequest timed out.\n");
//                cout << " *" << '\t' << "Request timed out." << endl;
                break;
            } else {
                break;
            }
        }
        iTTL++; //递增 TTL 值
    }
    return false;
}