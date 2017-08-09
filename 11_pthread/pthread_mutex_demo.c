#include "../apue.h"
#include <pthread.h>

struct foo
{
    int id;
    pthread_mutex_t f_lock;
    int f_count;
};
struct foo* foo_alloc(int id)
{
    struct foo* fp;
    if ((fp = malloc(sizeof(struct foo))) != NULL)
    {
        fp->id = id;
        fp->f_count = 1;
        if (pthread_mutex_init(&fp->f_lock, NULL) != 0)
        {
            free(fp);
            return NULL;
        }
    }
    return fp;
} 
void foo_hold(struct foo** fp)
{
    pthread_mutex_lock(&(*fp)->f_lock);
    (*fp)->f_count++;
    pthread_mutex_unlock(&(*fp)->f_lock);
}
void foo_rele(struct foo** fp)
{
    pthread_mutex_lock(&(*fp)->f_lock);
    if (--(*fp)->f_count == 0)
    {
        pthread_mutex_unlock(&(*fp)->f_lock);
        pthread_mutex_destroy(&(*fp)->f_lock);
        free((*fp));
        (*fp) = NULL;
    } 
    else 
    {
        pthread_mutex_unlock(&(*fp)->f_lock);
    }
}

void* thr_fn1(void* arg)
{
    printf("thread1: f_count = %d\n", (*(struct foo**)arg)->f_count);
    printf("thread1 id fooholding...\n\n");
    foo_hold(((struct foo**)arg));

    printf("thread1: f_count = %d\n", (*(struct foo**)arg)->f_count);
    printf("thread1 id fooholding...\n\n");
    foo_hold(((struct foo**)arg));

    pthread_exit((void*)1);
}
void* thr_fn2(void* arg)
{
    sleep(1);
    printf("thread2: f_count = %d\n", (*(struct foo**)arg)->f_count);
    printf("thread2 is fooreleing...\n\n");
    foo_rele(((struct foo**)arg));
    pthread_exit((void*)2);
}
int main()
{
    int err;
    pthread_t tid1, tid2;
    void* tret;
    struct foo* fp;

    if ((fp = foo_alloc(1)) == NULL)
        exit(0);
    err = pthread_create(&tid1, NULL, thr_fn1, &fp);
    if (err != 0)   err_exit(err, "cant create thread 1.");
    err = pthread_create(&tid2, NULL, thr_fn2, &fp);
    if (err != 0)   err_exit(err, "cant create thread 2.");
    sleep(2);
    while (fp != NULL)
    {
        printf("main thread: f_count = %d\n", fp->f_count);
        printf("main thread is fooreleing...\n\n");
        foo_rele(&fp);
    }
    printf("fp equal NULL ? : %d\n", (int)(fp == NULL));
    err = pthread_join(tid1, &tret);
    if (err != 0)   err_exit(err, "main thread cant join thread1.\n");
    printf("thread1 exit code: %d\n", (unsigned int)tret);
    err = pthread_join(tid2, &tret);
    if (err != 0)   err_exit(err, "main thread cant join thread2.\n");
    printf("thread2 exit code: %d\n", (unsigned int)tret);
    exit(0);
}