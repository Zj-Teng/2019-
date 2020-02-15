//
// Created by Administrator on 2019/12/11.
//

#ifndef TEST2_DEALDATA_H
#define TEST2_DEALDATA_H

#endif //TEST2_DEALDATA_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    int addr1;
    int addr2;
    int addr3;
    int addr4;
} Address;

int getIPAddress(Address *IP);
int convertToString(Address IP, char* addr);
void printIPAddress(Address IP);
BOOL isFinalAddress(Address IP1, Address IP2);

int getIPAddress(Address *IP){
    if (!IP)    return -1;
    char temp[255];
    scanf("%s", &temp);
    printf("%s\n", temp);
    sscanf(temp, "%d.%d.%d.%d", &IP->addr1, &IP->addr2, &IP->addr3, &IP->addr4);
    return 1;
}

int convertToString(Address IP, char* addr){
    char addr1[10];
    char addr2[10];
    char addr3[10];
    char addr4[10];

    itoa(IP.addr1, addr1, 10);
    itoa(IP.addr2, addr2, 10);
    itoa(IP.addr3, addr3, 10);
    itoa(IP.addr4, addr4, 10);

    sprintf(addr, "%s.%s.%s.%s", addr1, addr2, addr3, addr4);
    return 1;
}

void printIPAddress(Address IP){
    printf("IP:  %d.%d.%d.%d\n", IP.addr1,IP.addr2,IP.addr3,IP.addr4);
}

BOOL isFinalAddress(Address IP1, Address IP2){
    if (IP2.addr4 < IP1.addr4)
        return false;
    return true;
}