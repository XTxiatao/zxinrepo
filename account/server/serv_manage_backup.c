#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/stat.h>
#include "../includes/common.h"
/* 结构从声明定义和函数调用的分块改成思维顺序结构，视觉更混乱但是阅读审查更高效；且分支结构中视情况才需要用到的变量不会定义了而用不上 */

/* 进行用户数据备份的处理线程 */
void* backup(void *args)
{
    /* 必须使用且每次使用前需要初始化的变量，在循环体内定义，可以方便使用初始化器（生命周期为1轮循环，不会导致重复声明和定义）*/
    /* 循环使用且需要继承上轮一循环结果的变量在循环体外定义（不仅是声明，要初始化），循环体内只声明的话，初始值不确定；赋值或初始化又会导致不可继承残值 */
    while (1)
    {
        /* 启动后每隔指定时间备份一次 */
        sleep(BACKUPINTERVAL);
        /* 更新数据到二进制数据库文件中 */
        FILE *pBinaryData = NULL;
        pBinaryData = fopen(BINARYDATA,"w");
        if (NULL == pBinaryData)
        {

            printf("open %s failed\n", BINARYDATA);
            return 0;
        }
        fwrite(&g_tUser, sizeof(UserInfo_T), MAXUSER, pBinaryData);
        fclose(pBinaryData);
        /* 如果第一个日志文件太大，先腾挪数据到第二个日志文件 */ 
        struct stat tStatFirstLog = {0};
        stat(FIRSTLOG, &tStatFirstLog);
        if (tStatFirstLog.st_size > LOGSIZE)
        {
            //printf("firstlog size: %u, ready to update logs\n",tStatFirstLog.st_size);
            FILE *pFirstLog = NULL;
            pFirstLog = fopen(FIRSTLOG,"r");
            if (NULL == pFirstLog)
            {
                perror(NULL);
                printf("open %s  in read mode failed\n", FIRSTLOG);
                return 0;
            }
            char strBuff[BUFSIZ] = {};
            size_t dwCopySize;
            dwCopySize = fread(strBuff, 1, tStatFirstLog.st_size, pFirstLog);
            //printf("%u bytes copied to buffer\n", dwCopySize);
            fclose(pFirstLog);
            FILE *pSecondLog = NULL;
            pSecondLog = fopen(SECONDLOG,"w");
            if (NULL == pSecondLog)
            {
                perror(NULL);
                printf("open %s in write mode failed\n", SECONDLOG);
                return 0;
            }
            dwCopySize = fwrite(strBuff, 1, tStatFirstLog.st_size, pSecondLog);
            //printf("%u bytes write to %s\n", dwCopySize, SECONDLOG);
            fclose(pSecondLog);
            pFirstLog = fopen(FIRSTLOG,"w");
            if (NULL == pFirstLog)
            {
                perror(NULL);                
                printf("open %s in write mode failed\n", FIRSTLOG);
                return 0;
            }
            dwCopySize = fwrite(NULL, 0, 0, pFirstLog);
            //printf("%u bytes write to %s\n", dwCopySize, FIRSTLOG);

        }
        /* 更新数据到第一个日志 */
        FILE *pFirstLog = NULL;
        pFirstLog = fopen(FIRSTLOG, "a");
        if (NULL == pFirstLog)
        {
            perror(NULL);
            printf("update %s failed\n", FIRSTLOG);
        }
        time_t lTime = 0;
        time(&lTime);
        struct tm *pTime = NULL;
        pTime = gmtime(&lTime);
        if (NULL == pTime)
        {
            perror(NULL);
            printf("get time failed\n");
            return 0;
        }   
        fprintf(pFirstLog, "%-4d-%-02d-%-02d %-2d: %-2d: %-2d --------------------------\n", \
                            1900+pTime->tm_year, 1+pTime->tm_mon, pTime->tm_mday, 8+pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
        size_t total = 0;
        for (size_t i = 0; i < MAXUSER; i++)
        {
            if (g_tUser[i].iStat == 1)
            {
                total++;
                fprintf(pFirstLog, "user[%d]: %s\n", total, g_tUser[i].strUserName);
            }
        }
        fprintf(pFirstLog, "------------------------------------------------\n\n");
        fclose(pFirstLog);
    }
    pthread_exit(NULL);
}
