/*文件名采用小写单词加连字符，驼峰法可能在Linux下编译时找不到文件*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

/*定义全局互斥锁变量，可以直接此处用宏PTHREAD_MUTEX_INITIALIZER初始化赋值*/
pthread_mutex_t g_tMyMutex ;
/*定义全局条件变量，可以用宏PTHREAD_COND_INITIALIZER初始化赋值*/
pthread_cond_t g_tMyCond ;
/*用于条件判断的变量*/
int g_iNum = 0;
/* 接受信号的线程函数 */
void * waitForTrue(void *args) {
    /*使用pthread_cond_wait之前必须保证锁是被本线程加锁了的状态，所以先进行加锁操作*/
    sleep(1);
    int iRes = pthread_mutex_lock(&g_tMyMutex);
    if (0 != iRes)
    {
        printf("lock failed\n");
        pthread_exit("lock failed");
    }
    printf("wait thread lock first\n");
    /*pthread_cond_wait会立即解锁tMyMutex，然后判断等待pthread_cond_signal或者pthread_cond_broad向tMyCond发送解锁信号
     *tMyCond并不是作为一个被更改的变量，让pthread_cond_wait来根据值进行解锁的，它更像wait和signal函数的通道，由于wait不会主动去查找条件是否满足，所以即便tMyCond改变
     *也不会触发wait解除阻塞，所以如果signal发送信号早于wait函数阻塞，wait函数并不会直接解除阻塞，必须在wait阻塞之后再接收到signal的信号才能解除阻塞
     *解除阻塞之后，wait函数还会重新给tMymutex加锁，最终wait执行力解锁+加锁闭环，不影响外围加/解锁结构
     */
    if (pthread_cond_wait(&g_tMyCond, &g_tMyMutex) == 0) {
        printf("iNum = %d\n", g_iNum);
        /*解除阻塞后终止downForTrue线程*/
        iRes = pthread_cancel(*(pthread_t *)args);
        if (0 != iRes)
        {
            printf("waitForTrue cancel doneForTrue failed\n");
        }
        else
        {
            printf("waitForTrue cancel doneForTrue success\n");
        }
        
    }
    /*最终将互斥锁解锁*/
    pthread_mutex_unlock(&g_tMyMutex);
    pthread_exit(NULL);
}
/*发送信号的线程函数*/
void * doneForTrue(void *args) {
    int iRes;
    while (g_iNum != 10) {
        iRes = pthread_mutex_lock(&g_tMyMutex);
        if (iRes == 0) {
            g_iNum++;
            printf("doneForTrue: x = %d\n", g_iNum);
            sleep(1);
            pthread_mutex_unlock(&g_tMyMutex);
        }
    }
    /*每秒发送一次条件信号，避免wait线程加锁解锁等待信号之前signal已经发送完毕，导致wait一直阻塞*/
    /*循环不会自行退出，只能随着主线程return结束*/
    iRes = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (iRes != 0) {
        printf("set cancel state failed\n");
        return  NULL;
    }
    //设置线程接收到 Cancel 信号后立即结束执行
    iRes = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (iRes != 0) {
        printf("set cancel type failed\n");
        return  NULL;
    }
    while(1)
    {
        iRes = pthread_cond_broadcast(&g_tMyCond);
        if (0 != iRes)
        {
            printf("send signal to waitForTrue failed\n");
        }
        else
        {
            printf("send signal to waitForTrue success\n");
        }
        sleep(1);
    }
    pthread_exit(NULL);
}
int main() {
    /* 对互斥锁和条件变量初始化 */
    pthread_mutex_init(&g_tMyMutex, NULL);
    pthread_cond_init(&g_tMyCond, NULL);
    pthread_t dwWaitThread, dwSendThread;
    int iRes = pthread_create(&dwWaitThread, NULL, waitForTrue, &dwWaitThread);
    if (iRes != 0) {
        printf("mythread1 create failed \n");
        return 0;
    }
    iRes = pthread_create(&dwSendThread, NULL, doneForTrue, NULL);
    if (iRes != 0) {
        printf("mythread2 create failed \n");
        return 0;
    }
    /*等待 mythread1 线程执行完成*/
    iRes = pthread_join(dwWaitThread, NULL);
    if (iRes != 0) {
        printf("1：waiting mythread1 return failed\n");
    }
    else
    {
        printf("waitForTrue exit success\n");
    }
    
    iRes = pthread_cancel(dwSendThread);
    if (0 != iRes)
    {
        printf("main cancel doneForTrue failed\n");
    }
    else
    {
        printf("main cancel doneForTrue success\n");
    }
    
    //sleep(10);
    /*销毁条件变量*/
    pthread_cond_destroy(&g_tMyCond);
    return 0;
}
