// Copyright (c) 2015 Baidu.com , Inc. All Rights Reserved
// @author: yuanyi (yuan.yi.coding@gmail.com)
// @brief : 

#ifndef _INLINE_CASE_H_
#define _INLINE_CASE_H_

class TestCls
{
public:
    int func(int c)
    {
        m_c = c;    
        return m_c;
    }
private: 
    int m_c;
};

#endif

