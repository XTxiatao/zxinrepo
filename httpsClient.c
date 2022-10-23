/* 利用ssl编程，编译的时候需要手动链接ssl动态链接库，否则找不到其函数定义 */
/* 请更改宏为正确的配置，wtBuf内容为http GET 请求，可改为需要的报文*/
/* 编译时请加上-lcrypto -lssl以正确链接到动态库 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#define HOSTNAMELEN 50 
#define BUFFSIZE 1024
#define DEFAULTHOST "127.0.0.1"
#define DEFAULTPORT 9191
#define APPNAMELEN 20
#define CA_PEM "/keys/ca_cert.pem"
#define CERT_PEM "/keys/client_cert.pem"
#define KEY_PEM "/keys/client_key.pem"
int main(int argc, char *argv[])
{
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char host[HOSTNAMELEN] = DEFAULTHOST;
    int port = DEFAULTPORT;
    char appName[APPNAMELEN] = {0};
    char wtBuf[BUFFSIZE] = {0};
    char rdBuf[BUFFSIZE] = {0};
    if (argc < 2){
        printf("default connection: %s:%d\n\n", DEFAULTHOST, DEFAULTPORT);
    }
    if (argc == 2){
        strcpy(host, argv[1]);
    }else if (argc == 3){
        strcpy(host, argv[1]);
        port = atoi(argv[2]);
    }
    strcpy(appName, argv[0]);
    /* http get请求报文，请求服务器资源的关键 */
    sprintf(wtBuf, "GET / HTTP/1.1\r\nHost: %s\r\n\
        User-Agent: %s\r\nAccept: */*\r\n\r\n",
        host,
        appName
        );
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host);
    address.sin_port = htons(port);
    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr *)&address, len);
    if (result == -1)
    {
        perror("connection faild");
        printf("command format:%s  [ip [port]]\n", appName);
        exit(1);
    }

    /* 以下为成功创建socket后的ssl改造代码 */
    /* 首先初始化本地context ，设置好认证文件的路径*/
    SSL_library_init(); 
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX * ssl_ctx = SSL_CTX_new(TLS_client_method());
    int ret = SSL_CTX_load_verify_locations(ssl_ctx, CA_PEM, NULL);
    if (ret != 1){
        printf("ca load failed\n");
        exit(1);
    }
    ret = SSL_CTX_use_certificate_file(ssl_ctx, CERT_PEM, SSL_FILETYPE_PEM);
    if (ret != 1){
        printf("client_cert.pem load failed\n");
        perror(NULL);
        exit(1);
    }
    ret = SSL_CTX_use_PrivateKey_file(ssl_ctx, KEY_PEM, SSL_FILETYPE_PEM);
    if (ret != 1){
        printf("client_key.pem load failed\n");
        perror(NULL);
        exit(1);
    }
    ret = SSL_CTX_check_private_key(ssl_ctx);
    if (ret != 1){
        printf("client key and certificate match failed\n");
        exit(1);
    }
    SSL_CTX_set_mode(ssl_ctx, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_verify_depth(ssl_ctx, 1);
    /* 第二部，利用设置的context生成一个ssl会话 */
    SSL * ssl = SSL_new(ssl_ctx);
    /* 关键，将ssl会话与tcp socket绑定 */
    ret = SSL_set_fd(ssl, sockfd);
    if (ret != 1){
        printf("bind ssl and socket failed\n");
        exit(1);
    }
    /* 以下两部是通过ssl绑定的socket向服务端进行双向验证握手 */
    ret = SSL_connect(ssl);
    if (ret != 1){
        printf("ssl connect failed\n");
        exit(1);
    }
    ret = SSL_get_verify_result(ssl);
    if (ret  != X509_V_OK) {
        printf("Verification of handshake failed\n");
        exit(1);
    }
    printf("ssl connected...\n");
    /* 以下两行是原本无ssl加密的write和read函数对sockfd文件进行的读写，改为ssl函数对ssl会话的读写了 */
    SSL_write(ssl, wtBuf, strlen(wtBuf));
    SSL_read(ssl, rdBuf, sizeof(rdBuf));
    printf("%s\n", rdBuf);
    /* 以下三行是ssl加密通信结束后对ssl资源的释放 */
    SSL_shutdown(ssl);//关闭SSL套接字
    SSL_free(ssl);//释放SSL套接字
    SSL_CTX_free(ssl_ctx);//释放SSL会话环境
    close(sockfd);//释放socket      
    exit(0);
}
