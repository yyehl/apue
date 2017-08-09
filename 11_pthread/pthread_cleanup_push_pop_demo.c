#include "../apue.h"
#include <pthread.h>
void cleanup(void* arg)
{
    printf("cleanup : %s\n", (char*)arg);
}
void* thr_fn1(void* arg)
{
    printf("thread1 is starting.\n");
    pthread_cleanup_push(cleanup, "thread1 first cleanup.");
    pthread_cleanup_push(cleanup, "thread1 second cleanup.");
    if (arg)
        return ((void*)1);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    printf("thread1 cleanup is finished.\n");
    return ((void*)2);
}
void* thr_fn2(void* arg)
{
    printf("thread2 is starting.\n");
    pthread_cleanup_push(cleanup, "thread2 first cleanup.");
    pthread_cleanup_push(cleanup, "thread2 second cleanup.");
    if (arg)
        pthread_exit((void*)2);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    pthread_exit((void*)2);
}
int main()
{
    int err;
    pthread_t tid1, tid2;
    void* tret;
    err = pthread_create(&tid1, NULL, thr_fn1, (void*)1);
    if (err != 0)   err_exit(err, "cant create thread1.");
    err = pthread_create(&tid2, NULL, thr_fn2, (void*)1);
    if (err != 0)   err_exit(err, "cant create thread2.");
    err = pthread_join(tid1, &tret);
    if (err != 0)   err_exit(err, "main thread cant join thread1");
    printf("thread1 exit code %ld\n", (unsigned long)tret);
    err = pthread_join(tid2, &tret);
    if (err != 0)   err_exit(err, "main thread cant join thread2");
    printf("thread2 exit code %ld\n", (unsigned long)tret);
    exit(0);
}
