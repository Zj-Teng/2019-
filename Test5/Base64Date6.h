//
// Created by Administrator on 2019/12/14.
//

#ifndef TEST5_BASE64DATE6_H
#define TEST5_BASE64DATE6_H
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <winsock2.h>
#endif //TEST5_BASE64DATE6_H


typedef struct Base64Date6
{
    unsigned int d4:6;
    unsigned int d3:6;
    unsigned int d2:6;
    unsigned int d1:6;
}Base64Date6;

char ConvertToBase64  (char   c6);
void EncodeBase64     (char   *dbuf, char *buf128, int len);

char ConvertToBase64(char uc)
{
    if(uc < 26)
    {
        return 'A' + uc;
    }
    if(uc < 52)
    {
        return 'a' + (uc - 26);
    }
    if(uc < 62)
    {
        return '0' + (uc - 52);
    }
    if(uc == 62)
    {
        return '+';
    }
    return '/';
}

// base64的实现
void EncodeBase64(char *dbuf, char *buf128, int len)
{
    struct Base64Date6 *ddd      = NULL;
    int           i        = 0;
    char          buf[256] = {0};
    char          *tmp     = NULL;
    char          cc       = '\0';

    memset(buf, 0, 256);
    strcpy_s(buf, 256, buf128);
    for(i = 1; i <= len/3; i++)
    {
        tmp             = buf + (i-1)*3;
        cc              = tmp[2];
        tmp[2]          = tmp[0];
        tmp[0]          = cc;
        ddd             = (struct Base64Date6 *)tmp;
        dbuf[(i-1)*4+0] = ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i-1)*4+1] = ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i-1)*4+2] = ConvertToBase64((unsigned int)ddd->d3);
        dbuf[(i-1)*4+3] = ConvertToBase64((unsigned int)ddd->d4);
    }
    if(len % 3 == 1)
    {
        tmp             = buf + (i-1)*3;
        cc              = tmp[2];
        tmp[2]          = tmp[0];
        tmp[0]          = cc;
        ddd             = (struct Base64Date6 *)tmp;
        dbuf[(i-1)*4+0] = ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i-1)*4+1] = ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i-1)*4+2] = '=';
        dbuf[(i-1)*4+3] = '=';
    }
    if(len%3 == 2)
    {
        tmp             = buf+(i-1)*3;
        cc              = tmp[2];
        tmp[2]          = tmp[0];
        tmp[0]          = cc;
        ddd             = (struct Base64Date6 *)tmp;
        dbuf[(i-1)*4+0] = ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i-1)*4+1] = ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i-1)*4+2] = ConvertToBase64((unsigned int)ddd->d3);
        dbuf[(i-1)*4+3] = '=';
    }
    return;
}