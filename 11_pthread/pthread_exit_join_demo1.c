#include "../apue.h"
#include <pthread.h>

void* thr_fn1(void* arg)
{
    printf("thread1 is returning\n");
    return ((void*)1);
}
void* thr_fn2(void* arg)
{
    //sleep(1);
    printf("thread2 is returning\n");
    return ((void*)2);
}

int main(void)
{
    int         err;
    pthread_t   tid1, tid2;
    void*       tret;

    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if (err != 0)
        err_exit(err, "can't create thread1.");
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if (err != 0)
        err_exit(err, "can't create thread2.");
    
    err = pthread_join(tid1, &tret);
    if (err != 0)
        err_exit(err, "can't join with thread1.");
    printf("thread1 exit code %d\n", (int)tret);
    err = pthread_join(tid2, &tret);
    if (err != 0)
        err_exit(err, "can't join with thread2.");
    printf("thread2 exit code %ld\n", (long)tret);
    exit(0);
}