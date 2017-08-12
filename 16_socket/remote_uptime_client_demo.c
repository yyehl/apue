#include "../apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#define BUFLEN 128

extern int connect_retry(int, int, int, const struct sockaddr*, socklen_t);

void print_uptime(int sockfd)
{
    int n;
    char buf[BUFLEN];
    while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)  // TCP基于STREAM，不能一次性全部读取，需要循环
        write(STDOUT_FILENO, buf, n);               // 输出到标准输出
    if (n < 0)
        err_sys("recv error");
}

int main(int argc, char* argv[])
{
    struct addrinfo* aillist, *aip;
    struct addrinfo  hint;
    int sockfd, err;

    if (argc != 2)
        err_quit("usage: ruptime hostname");
    memset(&hint, 0, sizeof(hint));                     
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(argv[1], "ruptime", &hint, &aillist)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(err));
    for (aip = aillist; aip != NULL; aip = aip->ai_next)
    {
        if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0, aip->ai_addr, aip->ai_addrlen)) < 0)
            err = errno;         // 没连接上，返回错误
        else 
        {
            print_uptime(sockfd);   // 连上了，打印时间，返回
            exit(0);
        }
    }
}