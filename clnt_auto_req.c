#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include "../includes/common.h"

void * request(void *args)
{
    int order;
    for (order = 0; order < MAXCLIENTS; order++)
    {
        if (g_dwReqThread[order] == pthread_self())
        {
            break;
        }
    }
    int iAccountSock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in tServAddr;
    memset(&tServAddr, 0, sizeof(tServAddr));
    tServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    tServAddr.sin_family = AF_INET;
    tServAddr.sin_port = htons(3353);

    connect(iAccountSock, (struct sockaddr*)&tServAddr, sizeof(tServAddr));

    UserRequest_T tRequest = {};
    snprintf(tRequest.strUserName, MAXNAMELEN, "%ld", pthread_self());
    tRequest.byOperator = atoi(args);
    printf("Thread[%d]: %lu send operater: %d username: %s\n", order, pthread_self(), tRequest.byOperator, tRequest.strUserName);
    write(iAccountSock, &tRequest, sizeof(tRequest));
    UserResponse_T tResponse = {};
    memset(&tResponse, 0, sizeof(tResponse));
    read(iAccountSock, &tResponse, sizeof(tResponse));
    printf("Thread[%d] get response: %s\n", order, tResponse.strDescription);
    close(iAccountSock);
    g_dwReqThread[order] = 0;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    /* 用于接受函数返回值 */
    int iRes = 0;
    /* 设置处理线程属性，线程结束自动释放内存 */
    pthread_attr_t tThreadAttr;
    iRes = pthread_attr_init(&tThreadAttr);
    if (iRes != 0)
    {
        printf("init tThreadAttr failed\n");
    }
    iRes = pthread_attr_setdetachstate(&tThreadAttr, PTHREAD_CREATE_DETACHED);
    if (iRes != 0)
    {
        printf("set tThreadAttr detachstate failed\n");
    }
    int iReqNum = MAXREQUEST;
    while(iReqNum > 0)
    {
        for (int i = 0; i < MAXCLIENTS; i++)
        {
            /* 使用空闲线程发送请求接受回复 */
            if (g_dwReqThread[i] == 0)
            {
                if (argc == 1)
                {
                    printf("argument not enough\n");
                    return 0;
                }
                if (argc == 2)
                {
                    iRes = pthread_create(&g_dwReqThread[i], &tThreadAttr, request, argv[1]);                    
                }
                if (0 != iRes)
                {
                    printf("create thread failed\n");
                }
                iReqNum--;
                break;
            }
        }

    }
    pthread_exit(0);
}
