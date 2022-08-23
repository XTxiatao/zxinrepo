#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>   /* 使用time函数把全局数组的元素存储为时间值 */
#define REVIEWERS 2
#define WRITERS 4
#define PAGESIZE 10

long g_lBookPage[PAGESIZE];
int g_iPage = 0;
pthread_rwlock_t g_tMyrwlock = PTHREAD_RWLOCK_INITIALIZER;

void * reviewThread(void * args)
{
    int iRes = 0;
    /* 读的判断为<=而写的判断条件为< */
    while (g_iPage <= PAGESIZE)
    {
        iRes = pthread_rwlock_rdlock(&g_tMyrwlock);
        if (0 == iRes)
        {
            printf("%lu reading page %d, ",pthread_self(),g_iPage);
            printf("time of writing is %ld\n",g_lBookPage[g_iPage]);
            /* 解锁应及时且保证被执行到 */
            pthread_rwlock_unlock(&g_tMyrwlock);
            /* 写完后读完了最后一页就结束循环 */
            if (g_iPage == PAGESIZE)
            {
                pthread_exit("read all");
            }
        }
        else
        {
            printf("%lu can't read book\n",pthread_self());
            pthread_exit("read failed");
        }

        sleep(1);
    }
    pthread_exit(NULL);
}

void * writeThread(void * args)
{
    int iRes = 0;
    while (g_iPage < PAGESIZE)
    {
        iRes = pthread_rwlock_wrlock(&g_tMyrwlock);
        if (0 == iRes)
        {
            if (g_iPage < PAGESIZE)
            {
                /* 先增加页数再进行赋值，若顺序颠倒则读的时候读到的是还没有写入的下一页内容 */
                g_iPage++;
                g_lBookPage[g_iPage] = time(NULL);
                printf("write time is %ld\n",g_lBookPage[g_iPage]);
                printf("%lu have wrote Page %d\n",pthread_self(),g_iPage);
                sleep(1);
            }    
        }
        else
        {
            printf("%lu write book failed\n",pthread_self());
            pthread_exit("write failed");
        }
        pthread_rwlock_unlock(&g_tMyrwlock);
    }

    pthread_exit(NULL);   
}

int main(void)
{
    int iRes = 0;
    pthread_t tReviewer[REVIEWERS];
    pthread_t tWriter[WRITERS];

    for (int i = 0; i < WRITERS; i++)
    {
        iRes = pthread_create(&tWriter[i], NULL, writeThread, NULL);
        if (0 != iRes)
        {
            printf("create writer failed\n");
        }
    }
    for (int i = 0; i < REVIEWERS; i++)
    {
        iRes = pthread_create(&tReviewer[i], NULL, reviewThread, NULL);
        if (0 != iRes)
        {
            printf("create reviewer failed\n");
        }
    }
    /* 创建读写线程后记得阻塞主线程，接受子线程返回，否则主线程创建完子线程后就结束进程了 */
    for (int i = 0; i < WRITERS; i++)
    {
        iRes = pthread_join(tWriter[i],NULL);
        if (0 != iRes)
        {
            printf("get writerThread return failed\n");
        }
        else
        {
            printf("writer[%d] have down\n",i);
        }
    }
    for (int i = 0; i < REVIEWERS; i++)
    {
        iRes = pthread_join(tReviewer[i],NULL);
        if (0 != iRes)
        {
            printf("get reviewerThread return failed\n");
        }
        else
        {
            printf("reviewer[%d] have down\n",i);
        }
        
    }
    return 0;
}
