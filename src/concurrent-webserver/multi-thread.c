#include "multi-thread.h"
#include "request.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_THREADS 10

void task_queue_init(task_queue_t *q, int capacity)
{
    q->capacity = capacity;
    q->size = 0;
    q->front = 0;
    q->rear = -1;
    q->buffer = malloc(sizeof(int) * capacity);
    sem_init(&q->empty, 0, capacity);
    sem_init(&q->full, 0, 0);
    pthread_mutex_init(&q->mutex, NULL);
}

void task_queue_enqueue(task_queue_t *q, int item)
{
    sem_wait(&q->empty);
    pthread_mutex_lock(&q->mutex);
    q->rear = (q->rear + 1) % q->capacity;
    q->buffer[q->rear] = item;
    q->size++;
    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->full);
}

int task_queue_dequeue(task_queue_t *q)
{
    int item;
    sem_wait(&q->full);
    pthread_mutex_lock(&q->mutex);
    item = q->buffer[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->empty);
    return item;
}

void *worker(void *arg)
{
    task_queue_t *queue = (task_queue_t *)arg;
    while (1)
    {
        int conn_fd = task_queue_dequeue(queue);
        request_handle(conn_fd);
        close(conn_fd);
    }
}

void thread_pool_init(thread_pool_t *pool, int thread_count, task_queue_t *queue)
{
    pool->thread_count = thread_count;
    pool->task_queue = queue;
    for (int i = 0; i < thread_count; ++i)
    {
        pthread_create(&pool->threads[i], NULL, worker, queue);
    }
}
