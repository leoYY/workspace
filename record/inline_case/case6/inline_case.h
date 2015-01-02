// Copyright (c) 2015 Baidu.com , Inc. All Rights Reserved
// @author: yuanyi (yuan.yi.coding@gmail.com)
// @brief : 

#ifndef _INLINE_CASE_H_
#define _INLINE_CASE_H_

#include <stdio.h>

class TestCls
{
public:
    int func(int c)
    {
        printf("hehe :%d", c);
        return c;
    }
    int func1(int c)
    {
        for (int i = 0; i < 100; i++) 
        {
            c += i; 
        }
        printf("hehe :%d", c);
        return c;
    }
private: 
    int m_c;
};

#endif

