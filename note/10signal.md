# 10signal

* **信号**
信号是软件中断，信号提供了一种解决异步事件的方法
异步事件就是，当进程A需要等待某个事件的发生时，可以先去做其他的事，这个时间发生了，会来通知进程A，然后进程A再来处理这个事情

* **信号名**
信号名都被定义为正整数常量，都以SIG开头
比如SIGABRT，SIGALRM等。。
* **signal**：响应信号
表示捕捉某个特定的信号，并指定处理函数
捕捉的信号可能是内核发出的，也可能是其他进程发出的
* **kill**：
给指定的进程，发送指定的信号
```
#include <signal>
int kill(pid_t pid, int signo);
成功返回0，出错返回-1
```
* **alarm**
指定多少秒之后，给自己发送一个SIGALRM信号
但是如果到了指定时刻，不是论到你在执行，就会稍有延迟，轮到你才行
```
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
返回0或以前设置的闹钟余秒数
```
* **pause**
把进程暂时先挂起，直到捕捉到了一个信号，只有执行了信号处理程序才返回
```
#include <unistd.h>
int pause();
```
* **abort**
使进程异常退出，给自己发一个SIGABRT的信号，进程一般不会忽略此信号
```
#include <stdlib.h>
void abrt();
```
* **sleep**
把自己挂起，知道过了指定的时间，或者捕捉到了一个信号
与alarm一样，可能会稍微有点延迟
```
#include <unistd.h>
unsigned int sleep(unsigned int seconds);
```
