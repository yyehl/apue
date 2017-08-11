# 11pthread

* **线程**
进程资源分配最小单元，线程是CPU调度最小单元
一个进程可以创建多个线程，多个线程可共享该进程的地址空间
但是线程也会有自己的私有的寄存器，栈空间，信号屏蔽字等
单线程的进程与普通的进程一样

* **线程标识符**
线程与进程一样，也有自己唯一的标识符，但是只是在该进程内有效
进程标识符是数据结构pid_t
线程标识符是数据结构pthread_t

* **pthread_equal**：比较两个线程pthread_t是否相等
由于不同系统的数据结构pthread_t实现不一样，所以为了可移植性
需要使用pthread_equal函数来实现
```
#include <pthread.h>
int pthread_equal(pthread_t tid1, pthread_t tid2);
返回值：若相等，返回非0,；相等，返回0
参数1：第一个线程标识符
参数2：第二个线程标识符
```
* **pthread_self**：获取线程自己的标识符
```
#include <pthread.h>
pthread_t pthread_self(void);
返回值：返回自身线程ID
无参数
```

* **pthread_create**：创建线程
```
#include <pthread.h>
int pthread_create(pthread_t* restrict tidp, 
                   const pthread_attr_t *restrict attr,
                   void* (*start_rtn)(void*),
                   void* restrict arg);
返回值：若成功返回0；失败了返回错误编号
参数1：tidp为一个指针，指向要创建的线程tid的地址
参数2：用于指定各种不同的线程属性
参数3：新创建的线程从start_rtn函数地址开始执行，该函数只有一个无类型指针参数arg
参数4：如果start_rtn的参数不止一个，那么需要把所有的参数放在结构体中，把结构体的地址用参数arg传入start_rtn中
```

* **线程终止**：**pthread_exit(), pthread_join()**
任何一个线程调用了exit，_exit都会终止整个进程
单个线程终止的3个方式：
1）从线程函数返回，返回值是线程的退出码
2）线程可以被同一进程的其他线程取消
3）线程调用**pthread_exit**
```
#include <pthread.h>
void pthread_exit(void* rval_ptr);
返回值：无返回值
参数1：rval_ptr指向线程的终止信息，其他线程可以pthread_join访问到这个区域
```

```
#include <pthread.h>
int pthread_join(pthread_t thread, void** rval_ptr);
返回值：调用成功返回0，否则返回错误编号
参数1：等待的指定的线程pid
参数2：获取指向线程的终止状态，如不感兴趣可为NULL
```
对于没有detached分离的线程，在终止之后，会保留终止信息
如果没有pthread_join的话，就会造成内存泄露
所以没有分离的线程，必须被其他线程等待结束

pthread_exit传回的终止信息是由自己决定的，pthread_join只负责接收pthread_exit传回的指针
所以，pthread_exit必须传回一个其他线程能访问的地址，否则将可能会出错
比如线程1 exit 返回是传回了一个栈上的地址，那么线程2 join 接收这个参数，等线程2取读取这个参数时，线程1的栈已经被系统回收了，随时可能改变，所以结果会不可预知。

* **pthread_cancel**：取消进程
```
#include <pthread.h>
int pthread_cancel(pthread_t tid);
返回值：成功返回0，失败返回错误编号
参数：被请求取消的线程tid
```
线程A请求取消线程B，线程A并不等待线程B的取消
线程A只是提出请求，线程B怎么取消由线程B自己的属性决定

* **线程清理函数**：**pthread_cleanup_push, pthread_cleanup_pop**
线程可以指定在线程终止之前执行的线程清理函数
清理函数存在栈上，也就是说调用顺序与注册顺序相反
三种情况清理函数会调用：
1）调用pthread_exit
2）相应取消请求
2）pthread_cleanup_pop(execute)函数以非0参数调用
```
#include <pthread.h>
void pthread_cleanup_push(void (*rtn)(void*), void* arg);
void pthread_cleanup_pop(int execute);
无返回值
push：
参数1：表示要调用的清理函数
参数2：arg表示传递给处理函数的参数
pop：
参数1：如果execute为0，那么将不调用清理函数
但是不管execute参数怎么样，都将删除一个刚刚push的清理函数
```

* **进程与线程相关函数对比**
fork；pthread_create；创建新的控制流
exit；pthread_exit；从现有控制流中退出
waitpid；pthread_join；等待指定的控制流退出，得到其终止状态
atexit；pthread_cleanup_push；设置终止清理函数
getpid；pthread_self；得到自身控制流ID
abort；pthread_cancel；请求控制流的非正常退出

* **pthread_mutex**：互斥量

对于共享数据，需要使用互斥量来确保同一时刻只有一个线程访问数据
使用互斥量pthread_mutex_t来表示
进入临界区时加锁，从临界区返回时解锁
mutex的初始化与内存释放（仅当mutex是动态分配时才需要）
互斥量当尝试获取锁没有成功，会主动放弃CPU，等会再试
```
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t* restrict mutex, 
		                const pthread_mutexattr_t* restrict attr);
返回值：成功返回0，失败返回错误编号
参数1：mutex的地址
参数2：设置mutex的属性

int pthread_mutex_destroy(pthread_mutex_t* mutex);
返回值：成功返回0，失败返回错误编号
参数1：要被释放的mutex
```
pthread_mutex_lock：对互斥量加锁，尝试进入临界区。如果互斥量处于解锁状态，则加锁后进入临界区；如果互斥量已处于加锁状态，则被阻塞
pthread_mutex_unlock：对互斥量解锁
pthread_mutex_trylock：尝试对互斥量加锁，如果互斥量已被加锁，则直接返回0，而不阻塞

```
#include <pthraed.h>
int pthread_mutex_lock(pthread_mutex_t* mutex);
int pthread_mutex_unlock(pthread_mutex_t* mutex);
int pthread_mutex_trylock(pthread_mutex_t* mutex);
返回值：若成功返回0，失败返回错误编号
参数1：互斥量的地址
```

* **死锁避免**
1）控制互斥量的加锁顺序，总是对某个变量先加锁，对另一个后加锁
2）避免占用并等待，可以使用pthread_mutex_trylock来避免死锁


* **pthread_mutex_timedlock**
当试图加锁一个已上锁的mutex，会阻塞到指定的时间，若超过此事件，则返回错误码ETIMEDOUT
```
#include <pthread.h>
#include <time.h>
int pthread_mutex_timedlock(pthread_mutex_t* restrict mutex, 
							const struct timespec* restrict tsptr);
返回值：成功返回0，失败返回错误编号
参数1：指定mutex地址
参数2：指定等待到的绝对时间
```
* **pthread_rwlock_t**：读写锁
有三种状态，读者加锁状态，写者加锁状态，不加锁状态
读者加锁：允许其他读者进入，不允许写者进入
写者加锁：不允许其他读者进入，不允许其他写者进入
不加锁：没有读者也没有写者时
读写锁非常适合读次数远远大于写次数的场景

```
#include <pthread.h>
// 初始化
int pthread_rwlock_init(pthread_rwlock_t* restrict rwlock
					    const pthread_rwlockattr_t* restrict attr);
返回值：成功返回0，失败返回错误编号
参数1：要初始化的读写锁
参数2：指定读写锁的属性
// 释放锁
int pthread_rwlock_destroy(pthread_rwlock_t* rwlock);
返回值：成功返回0，失败返回错误参数
参数1：指定要被释放的读写锁
// 加锁与解锁
int pthread_rwlock_rdlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t* rwlock);
返回值：若成功返回0，失败返回错误编号
参数1：读写锁的地址
// 非阻塞式加锁，如不能获取锁，不阻塞，返回EBUSY错误
int pthread_rwlock_tryrdlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t* rwlock);
返回值：成功返回0，失败返回错误编号
参数1：指定读写锁的地址
// 带有超时的读写锁
int pthread_rwlock_timedrdlock(pthread_rwlock* restrict rwlock
								const struct timespec* restrict tsptr);
int pthread_rwlock_timedwrlock(pthread_rwlock* restrict rwlock
								const struct timespec* restrict tsptr);
返回值：成功返回0，失败返回错误编号
参数1：指定的读写锁地址
参数2：指定的绝对时间，在此时间之前不能解锁，则返回ETIMEDOUT错误
```

* **pthread_cond_t**：条件变量

1）条件变量给线程提供了一个会合的场所
2）条件变量需要与互斥量一起使用，互斥量保护条件变量
3）wait表示等待一个条件变量为真，如不为真，则线程被安排到一个等待队列
4）signal表示条件变量已经得到满足，把条件变量变为真，并通知一个等待队列的线程；broadcast可以通知等待队列的所有线程
5）互斥量作为参数传进函数，是为了保护条件变量；比如wait时，要先判断条件变量是否为真，不为真才放到等待队列，这期间可能会被打断，需要互斥量保护

```
#include <pthread.h>
// 条件变量的初始化
int pthread_cond_init(pthread_cond_t* restrict cond, pthread_mutex_t* restrict mutex);
返回值：成功返回0，失败返回错误编号
参数1：要初始化的条件变量
参数2：用于保护条件变量的互斥量
// 条件变量的释放
int pthread_cond_destroy(pthread_cond_t* cond);
返回值：成功返回0，失败返回错误编号
参数1：要被释放的条件变量
// wait操作
int pthread_cond_wait(pthread_cond_t* restrict cond, pthread_mutex_t* restrict mutex);
返回值：成功返回0，失败返回错误编号
参数1：等待的条件变量
参数2：用于保护条件变量的互斥量
// 限时的wait操作
int pthread_cond_timedwait(pthread_cond_t* restrict cond, 
							pthread_mutex_t* restrict mutex, 
							const struct timespec* restrict tsptr);
返回值：成功返回0，失败返回错误编号
参数1：等待的条件变量
参数2：用于保护条件变量的互斥量
参数3：绝对时间，在此时间内若条件变量还不为真，则返回错误编号
// signal操作，至少唤醒一个在等待队列的线程
int pthread_cond_signal(pthread_cond_t* cond);
返回值：成功返回0，失败返回错误编号
参数1：唤醒在该条件变量上等待的一个线程
// broadcast操作：唤醒在等待队列上的全部线程
int pthread_cond_broadcast(pthread_cond_t* cond);
返回值：成功返回0，失败返回错误编号
参数1：唤醒在该条件变量上等待的全部线程
```

* **自旋锁**：在获取锁之前一直尝试，忙等而阻塞状态

自旋锁使用场景：
锁被持有的时间短，线程不希望在重新调度上花太多成本
自旋锁常用于底层原语实现其他类型的锁
自旋锁一般用于多核处理器，单核不能用自旋锁
```
#include <pthread.h>
int pthread_spin_lock(pthread_spinlock_t* lock);
int pthread_spin_trylock(pthread_spinlock_t* lock);
int pthread_spin_unlock(pthread_spinlock_t* lock);
返回值：成功返回0， 失败返回错误编号
参数1：被尝试锁或解锁的的自旋锁
```
1）lock用于尝试获取锁，如未获取，则一直尝试
2）trylock用于尝试获取锁，如未获取，则返回错误RBUSY
3）unlock用于解锁

* **自旋锁与互斥量的区别**
1）
自旋锁如果没有获取锁，会一直占有CPU，一直尝试获取，处于忙等状态
互斥量如果没有获取锁，会暂时主动释放CPU，将自己挂起，稍后再尝试
2）
自旋锁适合锁被占有时间短的场景，避免线程调度产生大量开销
互斥量适合锁被占有时间更长的场景，避免一直尝试获取锁，占有CPU
3）
自旋锁不能用于单核处理器，因为当无法获取锁，而又一直处于忙等状态，别的线程没有机会释放锁，就成死锁状态了。适合多处理器
互斥量单核多核都行。

* **屏障**：提供多个线程会合的场所

屏障是用户协调多个线程并行工作的同步机制
允许任意多的线程等待，知道所有线程都达到某个点，然后再一起继续执行
```
#include <pthread.h>
// 初始化
int pthread_barrier_init(pthread_barrier_t* restrict barrier
						 const pthread_barrierattr_t* restrict attr
						 unsigned int count);
返回值：成功返回0，失败返回错误编号
参数1：需要初始化的屏障
参数2：屏障属性
参数3：指定需要等待线程的数量
// 释放
int pthread_barrier_destroy(pthread_barrier_t* barrier);
返回值：成功返回0，失败返回错误编号
参数1：要被释放的屏障
// wait操作：表示本线程已完成工作，等待其他线程赶上来
int pthread_barrier_wait(pthread_barrier_t* barrier);
```			
先到达的线程会进入休眠状态，等待其他线程的到达
当左右线程都到达时，屏障计数被满足，则所有线程被唤醒
当所有线程重新处理非阻塞状态，屏障可被重用
但是无法改变屏障计数，要改变计数只能重新初始化


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
