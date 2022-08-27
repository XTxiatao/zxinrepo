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

/* 处理新增用户请求 */
void useradd(char * userName, UserResponse_T *pResponse)
{
    printf("adding user...\n");
    for (int i = 0; i < MAXUSER; i++)
    {
        if (strcmp(userName,g_tUser[i].strUserName) == 0)
        {
            pResponse->byResult = 1;
            snprintf(pResponse->strDescription, MAXDESCRIPTION-1,"add user failed, user exist\n");
            break;
        }
        if (MAXUSER == i + 1)
        {
            for (int i = 0; i < MAXUSER; i++)
            {
                if (0 == g_tUser[i].iStat)
                {
                    g_tUser[i].iStat = 1;
                    strncpy(g_tUser[i].strUserName, userName,MAXNAMELEN-1);
                    pResponse->byResult = 0;
                    snprintf(pResponse->strDescription, MAXDESCRIPTION-1,"add user: %s success\n", userName);
                    break;
                }
                if (MAXUSER == i + 1)
                {
                    pResponse->byResult = 2;
                    snprintf(pResponse->strDescription, MAXDESCRIPTION-1,"add user failed, no space\n", userName);
                }
            }
        }
    }
}
