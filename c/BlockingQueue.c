#include "BlockingQueue.h"
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

// time convert and wrap sem_timedwait API
static int waitTimeOut(sem_t* sem, int timeout/* only support sec*/)
{
    struct timeval now;
    struct timespec time_out;
    gettimeofday(&now, NULL);
    
    {
        now.tv_sec += timeout;
        time_out.tv_sec  = now.tv_sec;
        time_out.tv_nsec = now.tv_usec * 1000;
    }
    return sem_timedwait(sem, time_out);
}

int push(BlockingQueue_t* queue, void* data, size_t size)
{
}

int push_by_timeout(BlockingQueue_t* queue, void* data, size_t size, int time_out /* sec..*/)
{
    if (queue == NULL || data == NULL)
        return RET_ERR;
    if (time_out <= 0)
        return push(queue, data, size);
    
    // NOTE must be do sem_wait before mutex_lock, or maybe deadlock.
    if ( queue->size == queue->length && 
            waitTimeOut(queue->full_sem, time_out) )
}

int destroyBloackingQueue(BlockingQueue_t* queue)
{
    int i;
    if (queue == NULL)
        return RET_ERR;
    // if we should to do freedom for node in queue.
    if (queue->size != 0)
    {
        for(i=0; i<queue->size ;i++ )
        {
            free queue->queue[(head+i)%queue->length];
            queue->queue[(head+i)%queue->length] = NULL;
        }
    }
    
    if (queue->full_sem != NULL)
        sem_destroy(queue->full_sem);
    if (queue->empty_sem != NULL)
        sem_destroy(queue->empty_sem);
    if (queue->mutex != NULL)
        pthread_mutex_destroy(queue->mutex);
    return RET_SUC;
}

int createBloackingQueue(BlockingQueue_t* queue, size_t init_size)
{
    if (init_size == 0 )                                        // init_size should be more than 0
        return RET_ERR;
    
    /**
     * We should not alloc memory for queue,
     * otherwise in destroy method we can't be sure ,whether to free this memory.
     *if (queue == NULL)
     *  queue = (BlockingQueue_t*) malloc(sizeof(BlockingQueue_t));
     */
     
    if (queue == NULL)                                          // malloc err, may be memory alloc err.
        return RET_ERR;
        
    memset(queue, 0, sizeof(BlockingQueue_t));                  // init mem to 0.
    queue->length = init_size;
    queue->size   = 0;
    do {
        queue->full_sem = (sem_t*) malloc(sizeof(sem_t));
        if (queue->full_set == NULL || 
                sem_init(queue->full_sem, 0, 0) < 0)
            break;
    
        queue->empty_sem = (sem_t*) malloc(sizeof(sem_t));
        if (queue->empty_sem == NULL ||
                sem_init(queue->empty_sem, 0, init_size) < 0)
            break;
        queue->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
        if (queue->mutex == NULL || 
                pthread_mutex_init(queue->mutex, NULL) != 0)
            break;
        return RET_OK;
    }while(0)
    
    destroyBlockingQueue(queue);                                // may any members init failed, destroy all.
    return RET_ERR;
}

