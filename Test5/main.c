#include <stdio.h>
#include "STMP.h"
#include "POP3.h"

#define LEN 255
#define CONTENTS_LEN 2048
#define TITLE_LEN 255
#define CONTENT_LEN 2000

void sendMes();
void recvMes();

int main()
{
    recvMes();
    //sendMes();
    return 0;
}

void sendMes(){
    char EmailTo[LEN] = "1269388615@qq.com";
    char EmailFrom[LEN] = "1269388615@qq.com";
    printf("请输入接收邮箱： ");
    scanf("%s", &EmailTo);
    printf("请输入发送邮箱： ");
    scanf("%s", &EmailFrom);
//    char EmailContents[CONTENT_LEN]   =
//                            "From: \"lucy\"<1269388615@qq.com>\r\n"
//                             "To: \"dasiy\"<1269388615@qq.com>\r\n"
//                             "Subject: Hello\r\n\r\n"
//                             "Hello World, Hello Email!";
    char EmailTitle[TITLE_LEN];
    printf("请输入邮件主题： ");
    scanf("%s", &EmailTitle);
    char EmailContent[CONTENT_LEN];
    printf("请输入邮件内容： ");
    getchar();
    fgets(EmailContent, CONTENT_LEN, stdin);
    char EmailContents[CONTENTS_LEN];
    sprintf(EmailContents, "From: \"lucy\"<%s>\r\n"
                           "To: \"dasiy\"<%s>\r\n"
                           "Subject: %s\r\n\r\n"
                           "%s", EmailFrom, EmailTo, EmailTitle, EmailContent);
    SendMail(EmailTo, EmailContents);
}

void recvMes(){
    recvMail();
}