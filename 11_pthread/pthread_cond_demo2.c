#include "../apue.h"
#include <pthread.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int num;

void* thr_fn1(void* arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        num++;
        printf("thread1: num = %d\n", num);
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        if (num >= 100)
        {
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&cond);
            break;
        }
        else
            pthread_mutex_unlock(&mutex);
    }
    pthread_exit((void*)1);
}
void* thr_fn2(void* arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        num += 2;
        printf("thread2: num = %d\n", num);
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        if (num >= 100)
        {
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&cond);
            break;
        }
        else
            pthread_mutex_unlock(&mutex);
    }
    pthread_exit((void*)2);
}
void* thr_fn3(void* arg)
{
    pthread_mutex_lock(&mutex);
    while (num < 100)
        pthread_cond_wait(&cond, &mutex);
    printf("ahaha, num = :%d\n", num);
    pthread_mutex_unlock(&mutex);
    pthread_exit((void*)3);
}

int main()
{
    int err;
    pthread_t tid1, tid2, tid3;
    void* tret;
    num = 0;
    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if (err != 0)   err_exit(err, "cant create thread 1.");
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if (err != 0)   err_exit(err, "cant create thread 1.");
    err = pthread_create(&tid3, NULL, thr_fn3, NULL);
    if (err != 0)   err_exit(err, "cant create thread 1.");
    sleep(2);

    printf("main thread: num = %d\n", num);
    err = pthread_join(tid1, &tret);
    if (err != 0)   err_exit(err, "main thread cant join thread 1.");
    printf("thread1 exit code: %d\n", (int)tret);
    err = pthread_join(tid2, &tret);
    if (err != 0)   err_exit(err, "main thread cant join thread 2.");
    printf("thread2 exit code: %d\n", (int)tret);
    err = pthread_join(tid3, &tret);
    if (err != 0)   err_exit(err, "main thread cant join thread 3.");
    printf("thread3 exit code: %d\n", (int)tret);
    exit(0);
}