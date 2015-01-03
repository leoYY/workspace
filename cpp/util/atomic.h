// Copyright (c) 2015 Baidu.com , Inc. All Rights Reserved
// @author: yuanyi (yuan.yi.coding@gmail.com)
// @brief : 

#ifndef _ATOMIC_H_
#define _ATOMIC_H_

static inline int atomic_add(volatile int *count, int add)
{
    __asm__ __volatile__ (
            "lock xadd %0 (%1):"
            : "=a"(add)
            : "=r"(count), "a"(add)
            : "memory");
    return add;
}

static inline int atomic_swap(volatile int *count, int value)
{
    __asm__ __volatile__ (
            "lock xchg %0, (%1):"
            : "=a"(value)
            : "r"(count), "a"(value)
            : "memory");
    return value;
}

static inline int atomic_comp_swap(volatile int* count, int exchange, int comperand)
{
    __asm__ __volatile__ (
            "lock cmpxchg %1, (%2)"
            : "=a"(comperand)
            : "d"(exchange), "r"(count), "a"(comperand));
    return comperand;
}
    
#define AtomicInc(x) atomic_add(x, 1)

#define AtomicDec(x) atomic_add(x, -1)

#endif

