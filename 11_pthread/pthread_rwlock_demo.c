#include "../apue.h"
#include <pthread.h>
#include <stdlib.h>

struct job
{
    struct job* j_next;
    struct job* j_prev;
    pthread_t   j_id;
};
struct queue
{
    struct job* q_head;
    struct job* q_tail;
    pthread_rwlock_t  q_lock;
};
int queue_init(struct queue* q)
{
    int err;
    q->q_head = NULL;
    q->q_tail = NULL;
    err = pthread_rwlock_init(&q->q_lock, NULL);
    if (err != 0)
        return err;
    return 0;
}
void job_insert(struct queue* q, struct job* jp)
{
    pthread_rwlock_wrlock(&q->q_lock);
    jp->j_next = q->q_head;
    jp->j_prev = NULL;
    if (q->q_head != NULL)
        q->q_head->j_prev = jp;
    else
        q->q_tail = jp;
    q->q_head = jp;
    pthread_rwlock_unlock(&q->q_lock);
}
void job_append(struct queue* q, struct job* jp)
{
    pthread_rwlock_wrlock(&q->q_lock);
    jp->j_prev = q->q_tail;
    jp->j_next = NULL;
    if (q->q_tail != NULL)
        q->q_tail->j_next = jp;
    else
        q->q_head = jp;
    q->q_tail = jp;
    pthread_rwlock_unlock(&q->q_lock);
}
void job_remove(struct queue* q, struct job* jp)
{
    pthread_rwlock_wrlock(&q->q_lock);
    if (jp->j_prev != NULL)
        jp->j_prev->j_next = jp->j_next;
    else 
        q->q_head = jp->j_next;
    if (jp->j_next != NULL)
        jp->j_next->j_prev = jp->j_prev;
    else 
        q->q_tail = jp->j_prev;
    pthread_rwlock_unlock(&q->q_lock);
}
struct job* job_find(struct queue* q, pthread_t id)
{
    struct job* jp;
    pthread_rwlock_rdlock(&q->q_lock);
    for (jp = q->q_head; (jp != NULL) && (jp->j_id != id); jp = jp->j_next)
    pthread_rwlock_unlock(&q->q_lock);
    return jp;
}