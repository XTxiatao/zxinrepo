#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include "../includes/serv_onethread.h"

UserInfo_T g_tUser[MAXUSER];

/* 创建监听socket的服务器程序入口 */
int main(void)
{
    /* 启动程序若有数据库，先从数据库中读取用户信息 */  
    FILE *binaryFP = NULL;
    binaryFP = fopen(BINARYDATA, "r");
    if (NULL != binaryFP)
    {
        fread(&g_tUser, 1, sizeof(UserInfo_T)*MAXUSER, binaryFP);
        /* 必须成功打开后才能调用fclose，否则程序可能崩溃 */
        fclose(binaryFP);
    }

    /* 用于接受函数返回值 */
    int iRes = 0;
    /* 监听请求的套接字 */
    int iAccount_sock = socket(AF_INET, SOCK_STREAM, 0);
    /* 设置套接字变量值 */
    struct sockaddr_in tServAddr;
    memset(&tServAddr,0,sizeof(tServAddr));
    tServAddr.sin_family = AF_INET;
    tServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    tServAddr.sin_port = htons(3353);
    /* 绑定套接字 */
    bind(iAccount_sock, (struct sockaddr*)&tServAddr, sizeof(tServAddr));
    /* 开始监听用户请求 */
    listen(iAccount_sock, MAXQUE);

    struct sockaddr_in tClntAddr;
    socklen_t dwClnt_addr_size = sizeof(tClntAddr);
    int iClnt_sock = accept(iAccount_sock, (struct sockaddr *)&tClntAddr, &dwClnt_addr_size);
    struct in_addr tServ_addr;
    struct in_addr tClnt_addr;
    memcpy(&tServ_addr, &tServAddr.sin_addr.s_addr, 4);
    memcpy(&tClnt_addr, &tClntAddr.sin_addr.s_addr, 4);
    printf("serv ip: %s     serv port: %hu\n", inet_ntoa(tServ_addr), ntohs(tServAddr.sin_port));//打印服务端的dw类型的ip值和port信息
    printf("clnt ip: %s     clnt port: %hu\n", inet_ntoa(tClnt_addr), ntohs(tClntAddr.sin_port));//打印客户端的ip和port信息（还原本来的w类型port值）
    UserRequest_T tRequest = {};
    UserResponse_T tResponse = {};
    while (1)
    {  
        int iSize = read(iClnt_sock, &tRequest, sizeof(tRequest));
        printf("received operater: %d username: %s\n", tRequest.byOperator, tRequest.strUserName);
        if (-1 == iSize)
        {
            printf("nothing received\n");
        }
        if (1 == tRequest.byOperator)
        {
            useradd(tRequest.strUserName, &tResponse);
        }
        else if (-1 == tRequest.byOperator)
        {
            userdel(tRequest.strUserName, &tResponse);
        }
        else if (0 == tRequest.byOperator)
        {
            userquery(tRequest.strUserName, &tResponse);
        }
        else
        {
            tResponse.byResult = 4;
            strncpy(tResponse.strDescription, "system error\n", MAXDESCRIPTION);
        }
        write(iClnt_sock, &tResponse, sizeof(UserResponse_T));
        printf("handled request!\n\n");
        memset(&tRequest, 0, sizeof(tRequest));
        memset(&tResponse, 0, sizeof(tResponse));
    }
    close(iClnt_sock);
    close(iAccount_sock);
    
    return 0;
}
