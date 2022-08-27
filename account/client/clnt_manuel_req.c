#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include "../includes/clnt_manuel.h"

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        printf("Too many arguments!\n");
        return 0;
    }
    if (argc == 2)
    {
        if (strcmp(argv[1],"--help") == 0)
        {
            printf("\n");
            printf("you can type %s to start the command\n", argv[0]);
            printf("after clnt>>> prompt, you can input one of these command:\n");
            printf("add/del/query XXX\n");
            printf("for example:\n");
            printf("add user1 to add a user1 to the database\n");
            return 0;
        }
        else
        {
            printf("wrong argument, please try --help to get more usage of the command\n");
            return 0;
        }
        
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(3353);  //端口
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    while (1)
    {
        UserRequest_T tRequest = {};
        UserResponse_T tResponse = {};
        printf("clnt>>>(input order or exit)");
        char order[6] = {};
        char name[20] = {};
        scanf("%s", order);
        if (strcmp(order, "add") == 0)
        {
            tRequest.byOperator = 1;
        }
        else if (strcmp(order, "del") == 0)
        {
            tRequest.byOperator = -1;
        }
        else if (strcmp(order, "query") == 0)
        {
            tRequest.byOperator = 0;
        }
        else
        {
            printf("clnt>>>wrong order: exit clnt\n");
            shutdown(sock, SHUT_RDWR);
            close(sock);
            return 0;
        }
        scanf("%s", name);
        strcpy(tRequest.strUserName, name);
        write(sock, &tRequest, sizeof(UserRequest_T));
        //读取服务器传回的数据
        char buffer[40];
        read(sock, &tResponse, sizeof(tResponse));
    
        printf("clnt>>>Message form server: %s\n", tResponse.strDescription);      
    }
    //关闭套接字
    close(sock);
    return 0;
}
