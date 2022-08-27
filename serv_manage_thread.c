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

/* 处理客户端请求的业务线程 */
void * handleThread(void *args)
{
    struct sockaddr_in tClnt_addr;
    socklen_t dwClnt_addr_size = sizeof(tClnt_addr);
    int iClnt_sock = accept(*(int *)args, (struct sockaddr *)&tClnt_addr, &dwClnt_addr_size);
    printf("clnt port: %hu\n", ntohs(tClnt_addr.sin_port));//打印客户端port信息（还原本来的w类型port值）
    int order;
    for (order = 0; order < MAXHANDLERS; order++)
    {
        if (g_dwResThread[order] == pthread_self())
        {
            printf("Thread[%d] is handling request...\n",order);
            break;
        }
    }
    UserRequest_T tRequest = {};
    UserResponse_T tResponse = {};
    while (1)
    {  
        int iSize = read(iClnt_sock, &tRequest, sizeof(tRequest));
        printf("received operater: %d username: %s\n", tRequest.byOperator, tRequest.strUserName);
        if (-1 == iSize)
        {
            printf("nothing received\n");
            g_dwResThread[order] = 0;
            pthread_exit(NULL);
            
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
    }
    close(iClnt_sock);
    printf("Thread[%d] have handled request!\n\n",order);
    g_dwResThread[order] = 0;

    pthread_exit(NULL);
}
