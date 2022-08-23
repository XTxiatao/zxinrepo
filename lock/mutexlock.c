/*文件名采用小写单词加连字符，驼峰法可能在Linux下编译时找不到文件*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define SELLERS 4
#define TIKECTS 10

/*总票数*/
int g_iTicketSum = TIKECTS;
/*全局互斥锁变量*/
pthread_mutex_t g_tMyMutex;                              

void * SellTicket(void * arg)
{
    unsigned long dwSold = 0;
    /*判断当前是否还有余票*/
    while (g_iTicketSum > 0)                             
    {
        /*有余票时排队进行枷锁*/
        int iIslock = pthread_mutex_lock(&g_tMyMutex);
        /*排队加锁期间票数量可能已经被其他seller售出，此处需要再判断一次*/
        if (g_iTicketSum > 0)                             
        {
            g_iTicketSum--;
            dwSold++;
            printf("%lu sell ticket %d \n",pthread_self(),TIKECTS-g_iTicketSum);
        }
        /*不要在加锁期间进行与保护变量无关的任务，阻塞其他线程执行*/
        /*sleep(1);*/           
        /*解锁操作必须在if条件外，否则可能不执行解锁导致其他线程一直被阻塞在等待加锁环节*/                          
        pthread_mutex_unlock(&g_tMyMutex);                 
    }
    /*sold会被强转为指针类型，应注意它不是有效指针，不要进行解引用，否则会执行错误*/
    pthread_exit(dwSold);                                 
}

int main(void)
{
    /*在所有线程使用之前初始化锁*/
    pthread_mutex_init(&g_tMyMutex,NULL);                  
    pthread_t dwSellers[SELLERS];
    for (int i = 0; i < SELLERS; i++)
    {
        int iIsCreated = pthread_create(&dwSellers[i],NULL,SellTicket,NULL);
        if (0 != iIsCreated)
        {
            printf("create thread failed\n");
        }
        else
        {
            printf("create thread id: %lu\n",dwSellers[i]);
        }
        
    }
    for (int i = 0; i < SELLERS; i++)
    {
        unsigned long dwSoldTickets;
        /*dwSoldTickets用来存储void *类型指针值，变量内存大小刚好为8，强转后完整存储到dwSoldTickets变量中*/
        int iIsDown = pthread_join(dwSellers[i],&dwSoldTickets);   
        if (0 != iIsDown)
        {
            printf("thread return failed\n");
        }
        else
        {
            /*dwSoldTickets中的数据被以 unsigned long 格式解读使用，刚好与出参前类型相同*/
            printf("seller%d sold %lu tickets \n",i,dwSoldTickets); 
        }
        
    }
    return 0;
}
