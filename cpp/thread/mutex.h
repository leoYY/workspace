
#include <pthread.h>
#include "noncopyable.h"

namespace common {
namespace thread {

class MutexLock: public noncopyable
{
public :
    MutexLock()
    {
        pthread_mutex_init(&_mutex, NULL); 
    }

    ~MutexLock()
    {
        pthread_mutex_destroy(&_mutex); 
    }

    void Lock()
    {
        pthread_mutex_lock(&_mutex); 
    }

    void UnLock()
    {
        pthread_mutex_unlock(&_mutex); 
    }

private : 
    pthread_mutex_t _mutex;
};

class MutexLockGuard : public noncopyable
{
public :
    explicit MutexLockGuard(MutexLock& mutex): _mutex(mutex)
    {
        _mutex.Lock(); 
    }

    ~MutexLockGuard()
    {
        _mutex.UnLock(); 
    }
private :
    MutexLock& _mutex;

};

}   // ending namespace thread
}   // ending namespace common
