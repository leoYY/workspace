/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file ngx_http_aret_module.h
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/03/29 20:56:13
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef PS_SPI_NGX_HTTP_ARET_MODULE_H
#define PS_SPI_NGX_HTTP_ARET_MODULE_H

#include "aret.h"

typedef struct {
    ngx_str_t web_table_name;
    ngx_str_t tag_table_name;
    ngx_str_t user_table_name;
} ngx_http_aret_db_conf_t;

typedef struct {
    // conf 
    ngx_str_t web_table_name;
    ngx_str_t tag_table_name;
    ngx_str_t user_table_name;

    // process shared mem
    teraeasy::Table* web_table ;        
    teraeasy::Table* user_table;
    teraeasy::Table* tag_table;
    aret::ReEngine* engine;     
} ngx_http_aret_process_ctx_t;

#endif  //PS_SPI_NGX_HTTP_ARET_MODULE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
