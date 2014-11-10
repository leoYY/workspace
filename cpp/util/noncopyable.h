/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file noncopyable.h
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/11/10 20:33:55
 * @version $Revision$ 
 * @brief just like boost/noncopyable
 *  
 **/
#ifndef PS_SPI_NONCOPYABLE_H
#define PS_SPI_NONCOPYABLE_H

namespace common {
namespace util {

class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable&);
    noncopyable& operator=(const noncopyable&);
};

}   // ending namespace util
}   // ending namespace common

#endif  //PS_SPI_NONCOPYABLE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
