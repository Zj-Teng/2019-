//
// Created by Administrator on 2019/12/13.
//

#ifndef TEST5_STMP_H
#define TEST5_STMP_H
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Base64Date6.h"
#include "OpenSock.h"
#endif //TEST5_STMP_H

void SendMail         (char   *email,char *body);

// 发送邮件
void SendMail(char *email, char *body)
{
    int     sockfd     = {0};
    char    buf[1500]  = {0};
    char    rbuf[1500] = {0};
    char    login[128] = {0};
    char    pass[128]  = {0};
    WSADATA WSAData;
    struct sockaddr_in their_addr = {0};
    WSAStartup(MAKEWORD(2, 2), &WSAData);
    memset(&their_addr, 0, sizeof(their_addr));

    their_addr.sin_family = AF_INET;
    their_addr.sin_port   = htons(25);
//    struct hostent* hptr = gethostbyname("smtp.163.com");
    struct hostent* hptr = gethostbyname("smtp.qq.com");
    memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
    printf("IP of smtp.qq.com is : %d:%d:%d:%d\n",
           their_addr.sin_addr.S_un.S_un_b.s_b1,
           their_addr.sin_addr.S_un.S_un_b.s_b2,
           their_addr.sin_addr.S_un.S_un_b.s_b3,
           their_addr.sin_addr.S_un.S_un_b.s_b4);

    // 连接邮件服务器，如果连接后没有响应，则2 秒后重新连接
    sockfd = OpenSocket((struct sockaddr *)&their_addr);
    memset(rbuf, 0, 1500);
    while(recv(sockfd, rbuf, 1500, 0) == 0)
    {
        printf("reconnect...\n");
//        cout<<"reconnect..."<<endl;
        Sleep(2);
        sockfd = OpenSocket((struct sockaddr *)&their_addr);
        memset(rbuf, 0, 1500);
    }

    printf("%s\n", rbuf);
//    cout<<rbuf<<endl;

    // EHLO
    memset(buf, 0, 1500);
    sprintf_s(buf, 1500, "EHLO HYL-PC\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("EHLO REceive: %s\n", rbuf);
//    cout<<"EHLO REceive: "<<rbuf<<endl;

    // AUTH LOGIN
    memset(buf, 0, 1500);
    sprintf_s(buf, 1500, "AUTH LOGIN\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("Auth Login Receive: %s\n", rbuf);
//    cout<<"Auth Login Receive: "<<rbuf<<endl;

    // USER
    memset(buf, 0, 1500);
    sprintf_s(buf, 1500, "1269388615@qq.com");//你的邮箱账号
    memset(login, 0, 128);
    EncodeBase64(login, buf, strlen(buf));
    sprintf_s(buf, 1500, "%s\r\n", login);
    send(sockfd, buf, strlen(buf), 0);
    printf("Base64 UserName: %s\n", buf);
//    cout<<"Base64 UserName: "<<buf<<endl;
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("User Login Receive: %s\n", rbuf);
//    cout<<"User Login Receive: "<<rbuf<<endl;

    // PASSWORD
    sprintf_s(buf, 1500, "lpmvvtnkqudljigf");//你的邮箱密码(授权码）
    memset(pass, 0, 128);
    EncodeBase64(pass, buf, strlen(buf));
    sprintf_s(buf, 1500, "%s\r\n", pass);
    send(sockfd, buf, strlen(buf), 0);
    printf("Base64 Password: %s\n", buf);
//    cout<<"Base64 Password: "<<buf<<endl;

    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("Send Password Receive: %s\n", rbuf);
//    cout<<"Send Password Receive: "<<rbuf<<endl;

    // MAIL FROM
    memset(buf, 0, 1500);
    sprintf_s(buf, 1500, "MAIL FROM: <1269388615@qq.com>\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("set Mail From Receive: %s\n", rbuf);
//    cout<<"set Mail From Receive: "<<rbuf<<endl;

    // RCPT TO 第一个收件人
    sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", email);
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("Tell Sendto Receive: %s\n", rbuf);
//    cout<<"Tell Sendto Receive: "<<rbuf<<endl;

    // DATA 准备开始发送邮件内容
    sprintf_s(buf, 1500, "DATA\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("Send Mail Prepare Receive: %s\n", rbuf);
//    cout<<"Send Mail Prepare Receive: "<<rbuf<<endl;

    // 发送邮件内容，\r\n.\r\n内容结束标记
    sprintf_s(buf, 1500, "%s\r\n.\r\n", body);
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("Send Mail Receive: %s\n", rbuf);
//    cout<<"Send Mail Receive: "<<rbuf<<endl;

    // QUIT
    sprintf_s(buf, 1500, "QUIT\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("Quit Receive %s\n", rbuf);
//    cout<<"Quit Receive: "<<rbuf<<endl;

    //清理工作
    closesocket(sockfd);
    WSACleanup();
    return;
}