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
/* 处理用户删除请求的功能函数 */
void userdel(const char *userName, UserResponse_T *pResponse)
{
    printf("deleting user...\n");
    for (int i = 0; i < MAXUSER; i++)
    {
        if (strcmp(userName, g_tUser[i].strUserName) == 0)
        {
            memset(&g_tUser[i], 0, sizeof(UserInfo_T));
            pResponse->byResult = 0;
            snprintf(pResponse->strDescription, MAXDESCRIPTION-1,"delete user: %s success\n", userName);
            break;
        }
        if (MAXUSER == i + 1)
        {
            pResponse->byResult = -1;
            snprintf(pResponse->strDescription, MAXDESCRIPTION-1,"delete user failed, user not exist\n");
        }
    }
}
