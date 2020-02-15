//
// Created by Administrator on 2019/12/14.
//

#ifndef TEST5_OPENSOCK_H
#define TEST5_OPENSOCK_H
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#endif //TEST5_OPENSOCK_H

int  OpenSocket(struct sockaddr *addr);

// 打开TCP Socket连接
int OpenSocket(struct sockaddr *addr)
{
    int sockfd = 0;
    sockfd=socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        printf("Open sockfd(TCP) error!\n");
//        cout<<"Open sockfd(TCP) error!"<<endl;
        exit(-1);
    }
    if(connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
    {
        printf("Connect sockfd(TCP) error!\n");
//        cout<<"Connect sockfd(TCP) error!"<<endl;
        exit(-1);
    }
    return sockfd;
}