/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file singleton.h
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/07/20 00:28:28
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef PS_SPI_SINGLETON_H
#define PS_SPI_SINGLETON_H

#include "mutex.h"

namespace common {
namespace util {

using ::common::thread::MutexLock;
using ::common::thread::MutexLockGuard;

class Singleton : public boost::noncopyable
{
private :
    static Singleton* _instance;
    static MutexLock _mutex;
public :   
    static Singleton* GetInstance()
    {
        if (_instance == NULL)         
        {
            MutexLockGuard mutex_guard(Singleton::_mutex);
            if (_instance == NULL)
            {
                _instance = new Singleton;     
            }
        }
        return _instance;
    }

    ~Singleton(){}
private :

    Singleton(){}
};

Singleton* Singleton::_instance = NULL;

}   // ending namespace util
}   // ending namespace common

#endif  //PS_SPI_SINGLETON_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
