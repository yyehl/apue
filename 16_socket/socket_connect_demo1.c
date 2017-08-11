// 处理连接错误的方法

#include "../apue.h"
#include <sys/socket.h>

#define MAXSLEEP 128

int connect_retry(int sockfd, const strcut sockaddr* addr, socklen_t len)
{
    int numsec;
    for (numsec = 1; numsec < MAXSLEEP; numsec <<= 1)
    {
        if (connect(sockfd, addr, len) == 0)
            return (0); 
        if (numsec <= MAXSLEEP/2)
            sleep(numsec);           //连接失败，稍后再试，每次等待的时间指数增加
    }
    return (-1);
}