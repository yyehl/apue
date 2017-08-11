# 16 socket

* **套接字描述符**

套接字是通信端点的抽象，被当做一种文件描述符，可以使用write，read
**创建**

```
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
返回值：成功返回套接字描述符，出错返回-1
参数1：确定通信特性，包括地址格式
参数2：确定套接字的类型
参数3：一般为0，表示给定域和给定类型下默认协议，若有多种，也可指定

关于参数1：套接字通信域，每个域都有自己表示地址的格式
AF_INET		IPv4因特网域
AF_INET6	IPv6因特网域
AF_UNIX		UNIX域
AF_UPSPEC	未指定

关于参数2：套接字类型
SOCK_DGRAM		固定长度，无连接，不可靠的报文传递（UDP）
SOCK_RAW	  	IP协议数据报接口，表示绕过TCP/UDP，自己负责构造头部，直通IP网络层
SOCK_SEQPACKET	固定长度，有序，可靠，面向连接的报文传递（SCTP）
SOCK_STREAM		有序，可靠，双向，面向连接的字节流（TCP）

关于参数3：为因特网套接字定义的协议
IPPROTO_IP		IPv4
IPPROTO_IPV6	IPv6
IPPROTO_ICMP	因特网控制报文协议
IPPROTO_RAW		原始IP数据报协议
IPPROTO_TCP		可靠传输控制协议
IPPROTO_UDP		用户数据报协议
```

**关闭**：套接字是双向的，可以用shutdown来禁止一个套接字的IO

```
#include <sys/socket.h>
int shutdown(int sockfd, int how);
返回值：成功返回0，失败返回-1
参数1：要禁止IO的socket套接字描述符
参数2：SHUT_RD关闭读端，SHUT_WR关闭写端，SHUT_RDWR关闭读写
```
**既然有close，为啥还需要shut_down呢**：暂时没有太理解，先记录
1）只有最后一个活动引用关闭时，close才释放网络网点，这意味着如果复制了（dup）一个套接字，知道最后一个引用它的文件描述符关闭才会真正释放这个套接字
2）shutdown可以是一个套接字描述符处于不活动状态，与描述符的引用计数无关
3）shutdown可以很方便的关闭套接字双向传输的一个方向


* **寻址**：目标通信进程有两部分标识：网络地址 + 端口号

**字节序**：大小端：linux是小端，但是TCP/IP协议栈是大端
大端：最低有效字节出现在最高字节地址上，即从高地址往低地址读
小端：最低有效字节出现在最低字节地址上，即从低地址往高地址读
可以使用ntoh或者hton实现网络字节序与主机字节序的转换

**地址格式**：一个地址标识一个特定通信域的套接字端点

```
// 通用套接字地址结构
struct socket_addr
{
	sa_family_t	ss_family;  // 地址族
	char		sa_data[];  // 可变长数组
};
// 不同系统地址结构有些不同，比如Linux的如下
struct sockaddr_t
{
	sa_family_t		sin_family;     // 地址族
	in_port_t		sin_port;	    //端口号
	struct in6_addr	sin6_addr;	    //IPv4地址 
	unsigned char	sin_zero[8];	// 填充字段，默认置为0
};
```
可使用inet_ntop将网络二进制字节序转化为文本字符串格式
可使用inet_pton将文本字符串格式转化为网络二进制字节序

**地址查询**
一系列函数可以查询主机，网络，服务，协议等等。。

**将套接字与地址关联**
需要给服务器绑定一个总所周知的地址：bind

```
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr* addr, socklen_t len);
返回值：成功返回0，出错返回-1
参数1：需要绑定地址的套接字描述符
参数2：被绑定在sockfd上的地址
参数3：地址长度
```

可以知道绑定在某个套接字上的地址：getsockname

```
int getsockname(int sockfd, struct sockaddr* restrict addr, socklen_t* restrict alenp);
返回值：成功返回0，失败返回-1
参数1：想知道绑定的地址的套接字描述符
参数2：绑定的地址存放的地方
参数3：指定存放地址的缓存区的长度
```

可以知道与套接字连接的对等方的地址

```
int getpeername(int sockfd, struct sockaddr* restrict addr, socklen_t* restrict alenp);
返回值：成功返回0，失败返回-1
参数1：想知道对等方地址的本方套接字描述符
参数2：对等方绑定的地址存放的地方
参数3：指定的存放对等方绑定地址的缓存区长度
```

* ****
* ****
* ****
* ****

* ****
* ****
* ****

* ****
* ****
* ****
* ****
* ****
* ****
* ****
* ****
* ****
* ****
* ****
* ****