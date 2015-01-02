// Copyright (c) 2015 Baidu.com , Inc. All Rights Reserved
// @author: yuanyi (yuan.yi.coding@gmail.com)
// @brief : 

#ifndef _INLINE_CASE_H_
#define _INLINE_CASE_H_

#include <stdio.h>

int inc(int a)
{
    return ++a;
}

int inc2(int b)
{
    printf("hehe %d\n", b);
    return  b;
}

#endif

