// Copyright (c) 2013 doodod.com , Inc. All Rights Reserved
// @author: yuanyi (yuan.yi.coding@gmail.com)
// @brief :  event-driver lib

#ifndef _AIO_H_
#define _AIO_H_

typedef struct _event_driver event_driver_t;
// proc
typedef int eventProc(event_driver_t* driver, int fd, void* clientData, int mask);
typedef int eventFinalizer(event_driver_t* driver, void *clientData);
typedef int eventBeforeSleepProc(event_driver_t* driver);

typedef struct _event {
    int mask;
    int fd;
    eventProc* readProc;
    eventProc* writeProc;
    void* clientData;
} event_t;

typedef struct _event_driver {
    int maxfd; 
    size_t length;
    int stop;
    event_t* events;
    event_t* fireds;
    eventFinalizer* finalizer;
    eventBeforeSleepProc* beforesleep;
} event_driver_t;

typedef enum _event_status {
    AE_NONE = 0,
    AE_READABLE,
    AE_WRITEBLE,
    AE_ALL;
} event_status_t;

typedef enum _ret_status {
    RET_SUC = 0,
    RET_ERR;
} ret_status_t;

int createEventDriver(event_driver_t* driver, size_t size);
int destroyEventDrivert(event_driver_t* driver);
int registEvent(event_driver_t* driver, int fd, void* clientData, int mask);
int unregistEvent(event_driver_t* driver, int fd, int delmask);
event_t* getEventByFd(event_driver_t* driver, int fd);
int process(event_driver_t* driver);


// API
extern int event_API_addEvent(evnet_driver_t* driver, int fd, int mask);
extern int event_API_delEvent(evnet_driver_t* driver, int fd, int delmask);
extern int event_API_poll(evnet_driver_t* driver, struct timeval *tvp);
extern int event_API_free(event_driver_t* driver);
extern int  event_API_resize(event_driver_t* driver);
extern int event_API_init(event_driver_t* driver);
extern int event_API_name(void);

#endif

