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

void userquery(char *userName, UserResponse_T *pResponse)
{
    printf("querying user...\n");
    for (int i = 0; i < MAXUSER; i++)
    {
        if (strcmp(userName, g_tUser[i].strUserName) == 0)
        {
            pResponse->byResult = 0;
            snprintf(pResponse->strDescription, MAXDESCRIPTION-1,"query user, %s exist\n", userName);
            break;
        }
        if (MAXUSER == i + 1)
        {
            pResponse->byResult = 3;
            snprintf(pResponse->strDescription, MAXDESCRIPTION-1,"query user: %s not exist\n",userName);
        }
    }
}
