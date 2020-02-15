//
// Created by Administrator on 2019/12/13.
//
#ifndef TEST5_POP3_H
#define TEST5_POP3_H
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#endif //TEST5_POP3_H


void recvMail();

// 发送邮件
void recvMail()
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
    their_addr.sin_port   = htons(110);//110为POP3的端口
    struct hostent* hptr = gethostbyname("pop.qq.com");
    memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
    printf("IP of POP3.qq.com is : %d:%d:%d:%d\n",
           their_addr.sin_addr.S_un.S_un_b.s_b1,
           their_addr.sin_addr.S_un.S_un_b.s_b2,
           their_addr.sin_addr.S_un.S_un_b.s_b3,
           their_addr.sin_addr.S_un.S_un_b.s_b4);
    //printf("1\n");
    // 连接邮件服务器，如果连接后没有响应，则2 秒后重新连接
    sockfd = OpenSocket((struct sockaddr *)&their_addr);
    memset(rbuf, 0, 1500);
    while(recv(sockfd, rbuf, 1500, 0) == 0)
    {
        printf("reconnect...\n");
//        cout<<"reconnect..."<<endl;
        Sleep(500);
        sockfd = OpenSocket((struct sockaddr *)&their_addr);
        memset(rbuf, 0, 1500);
    }

    printf("%s\n", rbuf);
//    cout<<rbuf<<endl;

    // EHLO
//    memset(buf, 0, 1500);
//    sprintf_s(buf, 1500, "EHLO HYL-PC\r\n");
//    send(sockfd, buf, strlen(buf), 0);
//    memset(rbuf, 0, 1500);
//    recv(sockfd, rbuf, 1500, 0);
//    printf("EHLO REceive: %s\n", rbuf);
//    cout<<"EHLO REceive: "<<rbuf<<endl;

    // AUTH LOGIN
//    memset(buf, 0, 1500);
//    sprintf_s(buf, 1500, "AUTH LOGIN\r\n");
//    send(sockfd, buf, strlen(buf), 0);
//    memset(rbuf, 0, 1500);
//    recv(sockfd, rbuf, 1500, 0);
//    printf("Auth Login Receive: %s\n", rbuf);
//    cout<<"Auth Login Receive: "<<rbuf<<endl;

    // USER
    memset(buf, 0, 1500);
    sprintf_s(buf, 1500, "user 1269388615@qq.com\r\n");//你的邮箱账号
    memset(login, 0, 128);
//    EncodeBase64(login, buf, strlen(buf));
//    sprintf_s(buf, 1500, "%s", login);
    send(sockfd, buf, strlen(buf), 0);
    printf("Base64 UserName: %s\n", buf);
//    cout<<"Base64 UserName: "<<buf<<endl;
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("User Login Receive: %s\n", rbuf);
//    cout<<"User Login Receive: "<<rbuf<<endl;

    // PASSWORD
    //lpmvvtnkqudljigf
    sprintf_s(buf, 1500, "pass lpmvvtnkqudljigf\r\n");//你的邮箱密码(授权码）
    memset(pass, 0, 128);
//    EncodeBase64(pass, buf, strlen(buf));
//    sprintf_s(buf, 1500, "%s", pass);
    send(sockfd, buf, strlen(buf), 0);
    printf("Base64 Password: %s\n", buf);
//    cout<<"Base64 Password: "<<buf<<endl;

    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("Send Password Receive: %s\n", rbuf);
//    cout<<"Send Password Receive: "<<rbuf<<endl;

    //向服务器发送请求
    memset(buf, 0, 1500);
    sprintf_s(buf, 1500, "stat\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("Mail Require From Service: %s\n", rbuf);

    //向服务器请求邮件大小
//    sprintf_s(buf, 1500, "list\r\n");
//    send(sockfd, buf, strlen(buf), 0);
//    memset(rbuf, 0, 1500);
//    recv(sockfd, rbuf, 1500, 0);
//    printf("Mail Size Receive: %s\n", rbuf);

    //获取前n行邮件
//    memset(buf, 0, 1500);
//    sprintf_s(buf, 1500, "TOP 1 2\r\n");
//    send(sockfd, buf, strlen(buf), 0);
//    memset(rbuf, 0, 1500);
//    recv(sockfd, rbuf, 1500, 0);
//    Sleep(1000);

    //获取指定邮件的内容
    memset(buf, 0, 1500);
    sprintf_s(buf, 1500, "retr 1\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    Sleep(1000); //等待数据接收完整
    recv(sockfd, rbuf, 1500, 0);
    printf("Receive data: %s\n", rbuf);

    // QUIT
    sprintf_s(buf, 1500, "quit\r\n");
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