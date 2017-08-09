#include "../apue.h"
#include <pthread.h>

#define NHASH 29
#define HASH(id)  (((unsigned long)id) % NHASH)

struct foo* fh[NHASH];
pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo 
{
    int f_count;
    pthread_mutex_t f_lock;
    int id;
    struct foo* next;
};

struct foo* foo_alloc(int id)
{
    int idx;
    struct foo* fp;
    if ((fp = malloc(sizeof(struct foo))) != NULL)
    {
        fp->f_count = 1;
        fp->id = id;
        if (pthread_mutex_init(&fp->f_lock, NULL) != 0)
        {
            free(fp);
            return NULL;
        }
        idx = HASH(id);
        pthread_mutex_lock(&hashlock);
        fp->next = fh[idx];
        fh[idx] = fp;
        pthread_mutex_lock(&fp->f_lock);
        pthread_mutex_unlock(&hashlock);
    
        pthread_mutex_unlock(&fp->f_lock);
    }
    return fp;
}

void foo_hold(struct foo* fp)
{
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->f_lock);
}

struct foo* foo_find(int id)
{
    struct foo* fp;
    pthread_mutex_lock(&hashlock);
    for (fp = fh[HASH(id)]; fp != NULL; fp = fp->next)
    {
        if (fp->id == id)
        {
            foo_hold(fp);
            break;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return fp;
}

void foo_rele(struct foo* fp)
{
    struct foo* tfp;
    int idx;
    pthread_mutex_lock(&fp->f_lock);
    if (fp->f_count == 1)
    {
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_lock(&hashlock);
        pthread_mutex_lock(&fp->f_lock);

        if (fp->f_count != 1)
        {
            fp->f_count--;
            pthread_mutex_unlock(&fp->f_lock);
            pthread_mutex_unlock(&hashlock);
            return ;
        }
        idx = HASH(fp->id);
        tfp = fh[idx];
        if (tfp == fp)
            fh[idx] = fp->next;
        else
        {
            while (tfp->next != fp)
                tfp = tfp->next;
            tfp->next = fp->next;
        }
        pthread_mutex_unlock(&hashlock);
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }
    else
    {
        fp->f_count--;
        pthread_mutex_unlock(&fp->f_lock);
    }

}


