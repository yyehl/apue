#include <pthread.h>
#include "../apue.h"
struct msg
{
    struct msg* m_next;
};
struct msg* workq;
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void process_msg(void)
{
    struct msg* mp;
    while (1)
    {                                               //最难理解的地方
        pthread_mutex_lock(&qlock);                 //要先加锁，因为下面需要判断workq是否为NULL，qlock保护的就是这个条件 
        while (workq == NULL)                       //wait操作，判断条件，等待，释放锁
            pthread_cond_wait(&qready, &qlock);     // 当等到了signal信号时，加锁，再次循环判断条件，可以执行了，就往下执行
        mp = workq;                                 //
        workq = mp->m_next;                         // 
        pthread_mutex_unlock(&qlock);               // 执行完了，需要解锁
    }
}
void enqueue_msg(struct msg* mp)
{
    pthread_mutex_lock(&qlock);      // 加锁
    mp->m_next = workq;
    workq = mp;
    pthread_mutex_unlock(&qlock);     // 处理完解锁
    pthread_cond_signal(&qready);     // 然后通知条件变量
}