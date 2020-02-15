#include <stdio.h>
#include "tracert.h"
#include "dealData.h"

Address* startAddress;
Address* currentAddress;
Address* endAddress;
HANDLE Mutex;

DWORD WINAPI ping(LPVOID lpParameter);

int main() {
    startAddress = (Address* ) malloc(sizeof(Address));
    currentAddress = (Address* ) malloc(sizeof(Address));
    endAddress = (Address* ) malloc(sizeof(Address));

    printf("������ʼIP��ַ:  ");
    getIPAddress(startAddress);
    printf("�������IP��ַ:  ");
    getIPAddress(endAddress);
    currentAddress->addr1 = startAddress->addr1;
    currentAddress->addr2 = startAddress->addr2;
    currentAddress->addr3 = startAddress->addr3;
    currentAddress->addr4 = startAddress->addr4;

    Mutex = CreateMutex(NULL,FALSE,NULL);
    HANDLE threads[20];
    for (int i = 0; i < 20; i ++){
        threads[i] = CreateThread(NULL, 0, ping, NULL, 0, NULL);
    }
    WaitForMultipleObjects(20, threads, true, INFINITE);

    return 0;
}

DWORD WINAPI ping(LPVOID lpParameter){
    while(true){
        char addr[255];
        Address temp = *(Address*) malloc(sizeof(Address));
        WaitForSingleObject(Mutex,INFINITE);
        if (!isFinalAddress(*currentAddress, *endAddress))
            return -1;
        temp = *currentAddress;
        //nextIPAddress(currentAddress, addr);
        currentAddress->addr4 ++;
        ReleaseMutex(Mutex);
        convertToString(temp, addr);
        printf("Ping  %s.....\n", addr);
        if (tracert(addr)){
            printf("%20s ����\n", addr);
        } else printf("%20s ������\n", addr);
    }
}
