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

* **请求建立连接**：connect

如果要处理面向连接的网络服务（TCP/SCTP），那在传输数据之前需要建立连接

```
int connect(int socketfd, const struct sockaddr* addr, socklen_t len);
返回值：成功返回0，出错返回-1
参数1：需要连接的本地套接字描述符，如该套接字未绑定地址，将为其绑定默认地址
参数2：要与之连接的的地址
参数3：地址长度
```

* **服务器监听**：listen

服务器用listen监听，表示愿意接收连接
调用listen后，才能接收到连接请求

```
int listen(int sockfd, int backlog);
返回值：成功返回0，失败返回-1
参数1：愿意接受连接的服务器进程套接字描述符
参数2：提示系统要入队的未完成连接请求数量，系统决定
```

* **接收连接**：accept

接收到连接请求后，服务器调用accept来接收连接请求，建立连接
需要强调的是，接受连接之后，会重新建立一个新的套接字与请求方通信
而原套接字仍继续监听连接请求
新套接字fd通过accept函数返回

```
int accept(int sockfd, struct sockaddr* restrict addr, socklen_t* restrict len);
返回值：成功返回新建立的套接字fd，失败返回-1
参数1：表示本地接收连接的套接字描述符
参数2：用于存放连接对等方地址的缓存，如不需要则为NULL
参数3：存放对等方地址的缓存长度，如不需要则为NULL
```


* **数据传输**：6个函数，3个发送，3个接收

1）send函数用于已经connect的套接字（UDP套接字connect了也行）

```
ssize_t send(int sockfd, const void* buf, size_t nbytes, int flags);
返回值：成功，返回发送的字节数，失败返回-1
参数1：已connect的套接字描述符
参数2：要发送的数据首地址
参数3：要传输的字节数
参数4：允许加入的标记

关于参数4：下面为Linux所支持的标记
MSG_CONFIRM		提供链路层反馈以保证地址映射有效
MSG_DONTROUTE	勿将数据包发送出本地路由
MSG_DONTWAIT	允许非阻塞操作（O_NONBLOCK）
MSG_EOR			标记记录的结束
MSG_MORE		延迟发送数据包允许写更多的数据
MSG_NOSIGNAL	在写无连接数据报时不产生SIGPIPE信号
MSG_OOB			如果协议支持，发送带外数据
```

2）sendto函数可以在无连接时指定一个目的地址

```
ssize_t sendto(int sockfd, const void* buf, size_t nbytes, int flags, 
				const struct sockaddr* destaddr, socklen_t destlen);
返回值：成功返回发送的字节数，出错返回-1
参数1：本地的socket描述符
参数2：要发送的数据首地址
参数3：要发送的数据长度
参数4：指定的标志，与send类似
参数5：发送的目的地址
参数6：目的地址的长度（不同格式目的地址长度 不同，所以要指定）
```

3）sendmsg，可以调用msghdr结构来指定多缓存区传输数据

```
ssize_t sendmsg(int sockfd, const struct msghdr* msg, int flags);
返回值：成功返回发送的字节数，失败返回-1
参数1：本地socket进程描述符
参数2：msghdr结构指针
参数3：指定标志，与send类似
```

4）recv，接收数据

```
ssize_t recv(int sockfd, void* buf, size_t nbytes, int flags);
返回值：成功返回接收到的字节数
	   若无可用数据或对方已按序结束，返回0
	   出错返回-1
参数1：本地已connect的socket进程标识符
参数2：存放接收数据缓存区地址
参数3：缓存区的最大长度
参数4：接收数据是可以指定的标志

关于参数4：下面是Linux支持的标志选择
MSG_CMSG_CLOEXEC	为UNIX域套接字上接收的文件标识符设置执行时关闭标志
MSG_DONTWAIT		启用非阻塞操作
MSG_ERRQUEUE		接收错误信息作为辅助数据
MSG_OOB				如果协议支持，接收带外数据
MSG_PEEK			返回数据包内容而不真正取走数据包
MSG_TRUNC			即使数据包被截断，也返回数据包的实际长度
MSG_WAITALL			等待知道所有数据可用（仅TCP）
```

5）recvfrom：如果对对等方的地址感兴趣，可以用recvfrom存下来

```
ssize_t recvfrom(int sockfd, void* restrict buf, size_t len, int flags
					strcut sockaddr* restrict addr, socklen_t* restrict addrlen);
返回值：成功返回接收的字节数
	   当无数据或对等方已经按序结束，则返回0
	   出错返回-1
参数1：本地接收数据的socket进程描述符
参数2：存放接收数据的缓存区首地址
参数3：缓存区的长度
参数4：指定的标志，与recv类似
参数5：用于存放对等方地址的地方
参数6：对等方地址的长度
```

6）recvmsg：为了将接收到的数据送入多个缓存区
		
```
ssize_t recvmsg(int sockfd, struct msghdr* msg, int flags);
返回值：成功返回数据的字节数
	   无可用数据或对等放已按序结束，返回0
	   出错返回-1
参数1：本地接收数据的socket描述符
参数2：msghdr结构指针
参数3：指定标志，与recv类似
```

* **套接字选项**

setsockopt用来设置选项，getsockopt用来获取选项

* **带外数据**

带外数据，指的是比普通数据优先级更高的数据
TCP中指的是紧急数据
但是TCP仅支持一个字节的紧急数据，在普通数据传递机制之外传输

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
