#ifndef _FILETRANS_
#define _FILETRANS_

typedef struct 
{
    char strUserName[20];
    int iUserId;
}UserInfo_T;

typedef struct 
{
    /* byOperator 为-1 表示删除用户，1表示新增用户*/
    signed char byOperator;
    char strUserName[20];
}UserQuery_T;

typedef struct 
{
    /* byResult 为 0 表示操作请求成功，为-1表示用户不存在删除失败，为1表示用户名已存在新增用户失败，为2表示用户数量已满新增失败 */
    signed char byResult;
    char strDescription[50];
}UserResponse_T;

#endif
