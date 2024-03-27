#ifndef MULTI_THREAD_H
#define MULTI_THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> // for malloc
#include <unistd.h> // for close

#define MAX_THREADS 10
#define MAX_QUEUE 100

typedef struct
{
    int *buffer;
    int capacity;
    int size;
    int front;
    int rear;
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
} task_queue_t;

typedef struct
{
    pthread_t threads[MAX_THREADS];
    int thread_count;
    task_queue_t *task_queue;
} thread_pool_t;

// Function prototypes
void task_queue_init(task_queue_t *q, int capacity);
void task_queue_enqueue(task_queue_t *q, int item);
int task_queue_dequeue(task_queue_t *q);
void *worker(void *arg);
void thread_pool_init(thread_pool_t *pool, int thread_count, task_queue_t *queue);

#endif /* THREAD_POOL_H */
