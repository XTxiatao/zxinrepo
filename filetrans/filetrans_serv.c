#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "filetrans.h"
#define MAXQUE 20
#define MAXUSER 20
#define MAXDESCRIPTION 50
#define MAXNAMELEN 20
/* 防止一个用户同时读/写文件 */
pthread_rwlock_t tMyrwlock = PTHREAD_RWLOCK_INITIALIZER;
UserInfo_T tUser[MAXUSER] = {};
int main(void)
{
    /* 监听请求的套接字 */
    int iServ_sock = socket(AF_INET, SOCK_STREAM, 0);
    /* 设置套接字变量值 */
    struct sockaddr_in tServAddr;
    memset(&tServAddr,0,sizeof(tServAddr));
    tServAddr.sin_family = AF_INET;
    tServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    tServAddr.sin_port = htons(3353);
    /* 绑定套接字 */
    bind(iServ_sock, (struct sockaddr*)&tServAddr, sizeof(tServAddr));
    /* 开始监听用户请求 */
    listen(iServ_sock, MAXQUE);
    /* 循环取队列中的请求 */
    while(1)
    {
        struct sockaddr_in tClnt_addr;
        socklen_t dwClnt_addr_size = sizeof(tClnt_addr);
        int iClnt_sock = accept(iServ_sock, (struct sockaddr *)&tClnt_addr, &dwClnt_addr_size);
        /* 等待增加多线程处理 */
        /* pthread_t dwServer */
        /* 读取客户端请求 */
        UserQuery_T tRequest = {};
        UserResponse_T tResponse = {};
        int iSize = 0;
        iSize = read(iClnt_sock, &tRequest, sizeof(tRequest));
        if (-1 == iSize)
        {
            printf("nothing in received\n");
        }
        /* 若请求删除用户 */
        if (-1 == tRequest.byOperator)
        {
            for (int i = 0; i < MAXUSER; i++)
            {
                if (strcmp(tRequest.strUserName,tUser[i].strUserName) == 0)
                {
                    memset(&tUser[i], 0, sizeof(UserInfo_T));
                    /* 增加写入日志记录 */
                    /* fwrite....... */
                    tResponse.byResult = 0;
                    snprintf(tResponse.strDescription, MAXDESCRIPTION-1,"delete user: %s success\n",tRequest.strUserName);
                    break;
                }
                if (MAXUSER == i + 1)
                {
                    tResponse.byResult = -1;
                    snprintf(tResponse.strDescription, MAXDESCRIPTION-1,"delete user failed, user not exist\n");
                }
            }
        }
        /* 若请求增加用户 */
        if (1 == tRequest.byOperator)
        {
            for (int i = 0; i < MAXUSER; i++)
            {
                if (strcmp(tRequest.strUserName,tUser[i].strUserName) == 0)
                {
                    tResponse.byResult = 1;
                    snprintf(tResponse.strDescription, MAXDESCRIPTION-1,"add user failed, user exist\n");
                }
                if (MAXUSER == i + 1)
                {
                    for (int i = 0; i < MAXUSER; i++)
                    {
                        if (0 == tUser[i].iUserId)
                        {
                            tUser[i].iUserId = i;
                            strncpy(tUser[i].strUserName, tRequest.strUserName,MAXNAMELEN-1);
                            tResponse.byResult = 0;
                            snprintf(tResponse.strDescription, MAXDESCRIPTION-1,"add user: %s success\n",tRequest.strUserName);
                            break;
                        }
                        if (MAXUSER == i + 1)
                        {
                            tResponse.byResult = 2;
                            snprintf(tResponse.strDescription, MAXDESCRIPTION-1,"add user failed, no space\n",tRequest.strUserName);
                        }
                    }
                }
            }

        }
        write(iClnt_sock, &tResponse, sizeof(UserResponse_T));
        close(iClnt_sock);
    }
    close(iServ_sock);

    return 0;
}
