// Copyright (c) 2015 Baidu.com , Inc. All Rights Reserved
// @author: yuanyi (yuan.yi.coding@gmail.com)
// @brief : 

#ifndef _INLINE_CASE_H_
#define _INLINE_CASE_H_

#include <stdio.h>

inline int inc(int a)
{
    return ++a;
}

inline int inc2(int b)
{
    for (int i = 0; i < 100; i++)
    {
        b += i; 
    }

    for (int i = 0; i < 10; i++)
    {
        b *= i; 
    }
    printf("hehe %d\n", b);
    return  b;
}

#endif

