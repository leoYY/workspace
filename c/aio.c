// Copyright (c) 2013 doodod.com , Inc. All Rights Reserved
// @author: yuanyi (yuan.yi.coding@gmail.com)
// @brief : 

#include "aio.h"

#include <stdlib.h>
#include <stdio.h>

static int processEvent(event_driver_t* driver)
{
    int ret = RET_SUC;
    f (driver == NULL)        
        return RET_ERR;
    if (driver->maxfd <= 0 ||driver->length <= 0) 
        return RET_SUC;
        
    // here assume all fields in event_driver_t is legal
    ret = 
}

int process(event_driver_t* driver)
{
    if(driver == NULL)
        return RET_ERR;

    while(driver->stop != 0)     
    {
        if (driver->beforesleep != NULL &&
               driver->beforesleep(driver) != RET_SUC)     
            break;
        if (processEvent(driver) != RET_SUC)             
            break;
    }
    return RET_ERR;
}

evnet_t* getEventByFd(event_driver_t* driver, int fd)
{
    if (driver == NULL ||
            driver->maxfd < fd )
        // fd wanted not exists.
        return NULL;

    if (driver->events[fd]->fd == 0)
        return NULL;

    return driver->events[fd];
}

int registEvent(event_driver_t* driver, int fd, void *clientData, int mask)
{
    if (fd <= 0 || driver == NULL)
        return RET_ERR;

    if (mask < 0 || mask >AE_ALL)
        return RET_ERR;

    if (fd > length)
        return RET_ERR;

    if (driver->events[fd] == NULL)
        return RET_ERR;

    event_t* event = driver->events[fd] ;
    // check if fd is already registed.
    if (event->fd != 0 && 
            (event->mask == mask || event->mask == AE_ALL))
        return RET_ERR;

    event->fd = fd;
    event->clientData = clientData;
    event->mask |= mask;
    if (driver->maxfd < fd)
        driver->maxfd = fd;
    return RET_SUC;
}

int unregistEvent(event_driver_t* driver, int fd, int delmask)
{
    if (driver == NULL || fd <= 0 )
        return RET_ERR;

    if (fd > driver->maxfd || 
            driver->events[fd] == 0)
        // not exists
        return RET_ERR
        
    event_t* event = driver->events[fd];
    if (event->mask & delmask == 0)
        // mask to del isn't exists
        return RET_ERR;

    event->mask ^= delmask;
    return RET_SUC;    
}

int createEventDriver(event_driver_t* driver, size_t size)
{
    if ( size <= 0)
        return RET_ERR;

    driver = (event_driver_t*) malloc(sizeof(event_driver_t));
    if (driver == NULL)
        return RET_ERR;
    memset(driver, 0, sizeof(event_driver_t));

    do {
    
        driver->events = (event_t*) malloc(sizeof(event_t)*size);
        if (driver->events == NULL)
            break;
        
        driver->fireds = (event_t*) malloc(sizeof(event_t)*size);
        if (driver->fireds == NULL)
            break;

        driver->length = size;
        return RET_SUC;
    }while (0)
     
    // do some failover
    if (driver->fireds != NULL)
        free(driver->fireds);

    if (driver->events != NULL)
        free(driver->events);

    if (driver != NULL)
        free(driver);

    return RET_ERR;
}

int destroyEventDrivert(event_driver_t* driver)
{
    if (driver->fireds != NULL)
        free(driver->fireds);

    if (driver->events != NULL)
        free(driver->events);

    if (driver != NULL)
        free(driver);

    return RET_SUC;
}
