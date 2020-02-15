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
    unsigned char hdr_len:4; //4 λͷ������
    unsigned char version:4; //4 λ�汾��
    unsigned char tos; //8 λ��������
    unsigned short total_len; //16 λ�ܳ���
    unsigned short identifier; //16 λ��ʶ��
    unsigned short frag_and_flags; //3 λ��־�� 13 λƬƫ��
    unsigned char ttl; //8 λ����ʱ��
    unsigned char protocol; //8 λ�ϲ�Э���
    unsigned short checksum; //16 λУ���
    unsigned long sourceIP; //32 λԴ IP ��ַ
    unsigned long destIP; //32 λĿ�� IP ��ַ
} IP_HEADER;

typedef struct {
    short sin_family;
    u_short sin_port;
    struct in_addr sin_addr;
    char sin_sero[8];
} sockaddr_in;

//ICMP ��ͷ
typedef struct {
    BYTE type; //8 λ�����ֶ�
    BYTE code; //8 λ�����ֶ�
    USHORT cksum; //16 λУ���
    USHORT id; //16 λ��ʶ��
    USHORT seq; //16 λ���к�
} ICMP_HEADER;

//���Ľ���ṹ
typedef struct {
    USHORT usSeqNo; //���к�
    DWORD dwRoundTripTime; //����ʱ��
    struct in_addr dwIPaddr; //���ر��ĵ� IP ��ַ
} DECODE_RESULT;

const int MAX_ICMP_PACKET_SIZE=1024;//ICMP������󳤶ȣ�������ͷ��
const int DEF_ICMP_DATA_SIZE=32;//ICMP����Ĭ�������ֶγ���
const int ICMP_ECHO_REQUEST=8;//ICMP�����ֶΣ�8��ʾ�������
const BYTE ICMP_ECHO_REPLY = 0; //����Ӧ��
const BYTE ICMP_TIMEOUT = 11; //���䳬ʱ
const int DEF_MAX_HOP = 30; //�����վ��
const DWORD DEF_ICMP_TIMEOUT = 3000; //����Ӧ��ʱʱ��

USHORT checksum(USHORT *pBuf, int iSize);
BOOL DecodeIcmpResponse(char *pBuf, int iPacketSize, DECODE_RESULT *DecodeResult, BYTE ICMP_ECHO_REPLY, BYTE ICMP_TIMEOUT);
BOOL tracert(char *IpAddress);
BOOL isValid(u_long* ulDestIP, char *IpAddress);

//��������У��ͺ���
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

//�����ݰ����н���
BOOL DecodeIcmpResponse(char *pBuf, int iPacketSize, DECODE_RESULT *DecodeResult, BYTE ICMP_ECHO_REPLY, BYTE ICMP_TIMEOUT) {
    //������ݱ���С�ĺϷ���
    IP_HEADER *pIpHdr = (IP_HEADER *) pBuf;
    int iIpHdrLen = pIpHdr->hdr_len * 4;
    if (iPacketSize < (int) (iIpHdrLen + sizeof(ICMP_HEADER)))
        return FALSE;
    //���� ICMP ����������ȡ ID �ֶκ����к��ֶ�
    ICMP_HEADER *pIcmpHdr = (ICMP_HEADER *) (pBuf + iIpHdrLen);
    USHORT usID, usSquNo;
    if (pIcmpHdr->type == ICMP_ECHO_REPLY) //ICMP ����Ӧ����
    {
        usID = pIcmpHdr->id; //���� ID
        usSquNo = pIcmpHdr->seq; //�������к�
    } else if (pIcmpHdr->type == ICMP_TIMEOUT)//ICMP ��ʱ�����
    {
        char *pInnerIpHdr = pBuf + iIpHdrLen + sizeof(ICMP_HEADER); //�غ��е� IP ͷ
        int iInnerIPHdrLen = ((IP_HEADER *) pInnerIpHdr)->hdr_len * 4; //�غ��е� IP ͷ��
        ICMP_HEADER *pInnerIcmpHdr = (ICMP_HEADER *) (pInnerIpHdr + iInnerIPHdrLen);//�غ��е� ICMP ͷ
        usID = pInnerIcmpHdr->id; //���� ID
        usSquNo = pInnerIcmpHdr->seq; //���к�
    } else {
        return false;
    }
    //��� ID �����к���ȷ���յ��ڴ����ݱ�
    if (usID != (USHORT) GetCurrentProcessId() || usSquNo != DecodeResult->usSeqNo) {
        return false;
    }
    //��¼ IP ��ַ����������ʱ��
    DecodeResult->dwIPaddr.s_addr = pIpHdr->sourceIP;
    DecodeResult->dwRoundTripTime = GetTickCount() - DecodeResult->dwRoundTripTime;
    //������ȷ�յ��� ICMP ���ݱ�
    if (pIcmpHdr->type == ICMP_ECHO_REPLY || pIcmpHdr->type == ICMP_TIMEOUT) {
        //�������ʱ����Ϣ
        if (DecodeResult->dwRoundTripTime) {
            printf("%dms", DecodeResult->dwRoundTripTime);
        } else {
            printf("<1ms");
        }
    }
    return true;
}

BOOL isValid(u_long* ulDestIP, char *IpAddress) {
    //ת�����ɹ�ʱ����������
    if (*ulDestIP == INADDR_NONE) {
        struct hostent *pHostent = gethostbyname(IpAddress);
        if (pHostent) {
            *ulDestIP = (*(struct in_addr *) pHostent->h_addr).s_addr;
            return true;
        } else {
            printf("����� IP ��ַ��������Ч!\n");
            WSACleanup();
            return false;
        }
    }
}

BOOL tracert(char *IpAddress) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
//    cout << "������һ�� IP ��ַ�������� ";
//    cin >> IpAddress;
    //�õ� IP ��ַ
    u_long ulDestIP = inet_addr(IpAddress);
    if (!isValid(&ulDestIP, IpAddress)){
        return false;
    }
//    cout << "Tracing route to " << IpAddress << " with a maximum of 30 hops.\n" << endl;
    //���Ŀ�ض� socket ��ַ
    sockaddr_in destSockAddr;
    ZeroMemory(&destSockAddr, sizeof(sockaddr_in));
    destSockAddr.sin_family = AF_INET;
    destSockAddr.sin_addr.s_addr = ulDestIP;
    //����ԭʼ�׽���
    SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0,
                               WSA_FLAG_OVERLAPPED);
    //��ʱʱ��
    int iTimeout = 500;
    //���ճ�ʱ
    setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout));
    //���ͳ�ʱ
    setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout));
    //���� ICMP ����������Ϣ������ TTL ������˳���ͱ���
    //��� ICMP ������ÿ�η���ʱ������ֶ�
    char IcmpSendBuf[sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE];//���ͻ�����
    memset(IcmpSendBuf, 0, sizeof(IcmpSendBuf)); //��ʼ�����ͻ�����
    char IcmpRecvBuf[MAX_ICMP_PACKET_SIZE]; //���ջ�����
    memset(IcmpRecvBuf, 0, sizeof(IcmpRecvBuf)); //��ʼ�����ջ�����
    ICMP_HEADER *pIcmpHeader = (ICMP_HEADER *) IcmpSendBuf;
    pIcmpHeader->type = ICMP_ECHO_REQUEST; //����Ϊ�������
    pIcmpHeader->code = 0; //�����ֶ�Ϊ 0
    pIcmpHeader->id = (USHORT) GetCurrentProcessId(); //ID �ֶ�Ϊ��ǰ���̺�
    memset(IcmpSendBuf + sizeof(ICMP_HEADER), 'E', DEF_ICMP_DATA_SIZE);//�����ֶ�
    USHORT usSeqNo = 0; //ICMP �������к�
    int iTTL = 1; //TTL ��ʼֵΪ 1
    BOOL bReachDestHost = FALSE; //ѭ���˳���־
    int iMaxHot = DEF_MAX_HOP; //ѭ����������
    DECODE_RESULT DecodeResult; //���ݸ����Ľ��뺯���Ľṹ������
    while (!bReachDestHost && iMaxHot--) {
        //���� IP ��ͷ�� TTL �ֶ�
        setsockopt(sockRaw, IPPROTO_IP, IP_TTL, (char *) &iTTL, sizeof(iTTL));
        //printf("%d", iTTL);
//        cout << iTTL << flush; //�����ǰ���
        //��� ICMP ������ÿ�η��ͱ仯���ֶ�
        ((ICMP_HEADER *) IcmpSendBuf)->cksum = 0; //У�������Ϊ 0
        ((ICMP_HEADER *) IcmpSendBuf)->seq = htons(usSeqNo++); //������к�
        ((ICMP_HEADER *) IcmpSendBuf)->cksum = checksum((USHORT *) IcmpSendBuf,
                                                        sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE); //����У���
        //��¼���кź͵�ǰʱ��
        DecodeResult.usSeqNo = ((ICMP_HEADER *) IcmpSendBuf)->seq; //��ǰ���
        DecodeResult.dwRoundTripTime = GetTickCount(); //��ǰʱ��
        //���� TCP ����������Ϣ
        sendto(sockRaw, IcmpSendBuf, sizeof(IcmpSendBuf), 0, (struct sockaddr *) &destSockAddr, sizeof(destSockAddr));
        //���� ICMP ����Ĳ����н�������
        sockaddr_in from; //�Զ� socket ��ַint iFromLen=sizeof(from); //��ַ�ṹ��С
        int iFromLen=sizeof(from); //��ַ�ṹ��С
        int iReadDataLen; //�������ݳ���
        while (1) {
            //��������
            iReadDataLen = recvfrom(sockRaw, IcmpRecvBuf, MAX_ICMP_PACKET_SIZE, 0, (struct sockaddr *) &from, &iFromLen);
            if (iReadDataLen != SOCKET_ERROR)//�����ݵ���
            {
                //�����ݰ����н���
                if (DecodeIcmpResponse(IcmpRecvBuf, iReadDataLen, &DecodeResult, ICMP_ECHO_REPLY, ICMP_TIMEOUT)) {
                    //����Ŀ�ĵأ��˳�ѭ��
                    if (DecodeResult.dwIPaddr.s_addr == destSockAddr.sin_addr.s_addr)
                        bReachDestHost = true;
                    //��� IP ��ַ
                    //printf("%20s\n", inet_ntoa(DecodeResult.dwIPaddr));
                    return true;
//                    cout << '\t' << inet_ntoa(DecodeResult.dwIPaddr) << endl;
//                    break;
                }
            } else if (WSAGetLastError() == WSAETIMEDOUT) //���ճ�ʱ�����*��
            {
                //printf(" *\tRequest timed out.\n");
//                cout << " *" << '\t' << "Request timed out." << endl;
                break;
            } else {
                break;
            }
        }
        iTTL++; //���� TTL ֵ
    }
    return false;
}