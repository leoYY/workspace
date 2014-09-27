/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file src/process_context.h
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/03/28 20:09:49
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef PS_SPI_SRC_PROCESS_CONTEXT_H
#define PS_SPI_SRC_PROCESS_CONTEXT_H

#include <string>
#include <map>

class ProcessContext
{
public :
    static ProcessContext* getInstance()
    {
        static ProcessContext ctx; 
        return &ctx;
    }

    inline void set(const std::string& key, void* data)
    {
        __ctx_data.insert(std::pair<std::string, void*>(key, data));     
    }

    /**
     * @brief return NULL if not exists
     *
     * @param [in] key   : const std::string&
     * @return  void* 
     * @retval   
     * @see 
     * @author yuanyi03
     * @date 2014/03/28 20:13:02
    **/
    inline void* get(const std::string& key) 
    {
        std::map<std::string, void*>::iterator it = __ctx_data.find(key); 
        if (it == __ctx_data.end())
        {
            return NULL; 
        }
        return it->second;
    }
private :
    std::map<std::string, void*> __ctx_data;
};

#endif  //PS_SPI_SRC/PROCESS_CONTEXT_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
