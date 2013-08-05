// just a threadsafe blockingQueue

#ifndef _BLOCKING_QUEUE_H
#define _BLOCKING_QUEUE_H

#include <sys/time.h>
#include <sempaphore.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum RET{
    RET_SUC = 0,
    RET_ERR ;
};

typedef struct _BlockingQueue_Node{
    void* content;              // blockingqueue node only store a pointer to content
    size_t size;
}node_t;

typedef struct _BlockingQueue{
    size_t size;
    size_t length;
    sem_t* full_sem;
    sem_t* empty_sem;
    pthread_cond_t* full_cond;
    pthread_cond_t* empty_cond;
    pthread_mutex_t* mutex;
    int head;
    int tail;
    node_t* queue;
    unsigned int isInit:1;
}BlockingQueue_t;

int push_by_timeout(BlockingQueue_t* queue, void* data, size_t size, int time_out /* sec..*/);
int push(BlockingQueue_t* queue, void* data, size_t size);

int pop_by_timeout(BlockingQueue_t* queue, void* data, size_t size, int time_out);
int pop(BlockingQueue_t* queue, void* data, size_t size);

int createBloackingQueue(BlockingQueue_t* queue, size_t init_size);
int destroyBloackingQueue(BlockingQueue_t* queue);

#define GETSIZE(queue) queue->size;
#define GETLENGTH(queue) queue->length;

#endif
