#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include "../includes/serv_manage.h"

pthread_t g_dwResThread[MAXHANDLERS];
pthread_t g_dwReqThread[MAXCLIENTS];
pthread_rwlock_t g_tMyrwlock;
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
    /* 初始化读写锁 */
    pthread_rwlock_init(&g_tMyrwlock, NULL);
    /* 创建备份数据线程 */
    pthread_t backThread;
    iRes = pthread_create(&backThread, NULL, backup, NULL);
    if (iRes != 0)
    {
        printf("create backup thread failed\n");
    }
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

    /* 循环取队列中的请求 */
    while(1)
    {
        for (int i = 0; i < MAXHANDLERS; i++)
        {
            /* 使用空闲线程处理请求 */
            if (g_dwResThread[i] == 0)
            {
                iRes = pthread_create(&g_dwResThread[i], &tThreadAttr, handleThread, &iAccount_sock);
                if (0 != iRes)
                {
                    printf("create thread failed\n");
                }
                break;
            }
        }
    }
    close(iAccount_sock);

    return 0;
}
