// 分配和初始化套接字供服务器进程使用

#include "../apue.h"
#include <errno.h>
#include <sys/socket.h>

int initserver(int type, const struct sockaddr* addr, socklen_t alen, int qlen)
{
    int fd;
    int err = 0;
    if ((fd = socket(addr->sa_family, type, 0)) < 0)   // 创建套接字
        return (-1);
    if ((bind(fd, addr, alen)) < 0)                    // 套接字绑定地址
        goto errout;
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) // 如果是面向连接的套接字
        if (listen(fd, qlen) < 0)                      // 那么就开始监听 
            goto errout;
    return fd;                                         // 返回创建好了的，已经开始监听的套接字fd

    errout:
        err = errno;
        close(fd);
        errno = err;
        return (-1);
}