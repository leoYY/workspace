// Copyright (c) 2015 Baidu.com , Inc. All Rights Reserved
// @author: yuanyi (yuan.yi.coding@gmail.com)
// @brief : 

#ifndef _INLINE_CASE_H_
#define _INLINE_CASE_H_

class TestCls
{
public:
    inline int func(int c);
    int func1(int c);
private: 
    int m_c;
};

int TestCls::func(int c)
{
    m_c = c;
    return c;
}

int TestCls::func1(int c)
{
    m_c = c;
    return c;
}

#endif

