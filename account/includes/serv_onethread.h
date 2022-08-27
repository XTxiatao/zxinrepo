#ifndef _FILETRANS_ACCOUNT_H_
#define _FILETRANS_ACCOUNT_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define MAXQUE 100
#define MAXUSER 100
#define MAXDESCRIPTION 50
#define MAXNAMELEN 20
#define MAXHANDLERS 5
#define MAXCLIENTS 100
#define MAXREQUEST 300
#define BACKUPINTERVAL 10
#define LOGSIZE 4800
#define FIRSTLOG "../logs/useraccount.txt"
#define SECONDLOG "../logs/useraccount1.txt"
#define BINARYDATA "../logs/useraccount.data"

typedef struct 
{   
    /* iStat为0表示账户空间未被使用，为1表示已被使用 */
    char strUserName[20];
    int iStat;
}UserInfo_T;
typedef struct 
{
    /* byOperator 为-1 表示删除用户，1表示新增用户, 0表示查询用户*/
    signed char byOperator;
    char strUserName[20];
}UserRequest_T;
typedef struct 
{
    /* byResult 为 0 表示操作请求成功，为-1表示用户不存在删除失败，为1表示用户名已存在新增用户失败，为2表示用户数量已满新增失败, 3表示查询用户不存在, 4表示系统错误 */
    signed char byResult;
    char strDescription[50];
}UserResponse_T;

void useradd(char * userName, UserResponse_T *pResponse);
void userdel(const char *userName, UserResponse_T *pResponse);
void userquery(char *userName, UserResponse_T *pResponse);

/* 全局数据存储用户信息 */
extern UserInfo_T g_tUser[MAXUSER];

#endif