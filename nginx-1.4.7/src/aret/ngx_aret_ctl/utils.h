/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file utils.h
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/03/29 16:36:07
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef PS_SPI_UTILS_H
#define PS_SPI_UTILS_H

extern "C" {
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
}

#include <map>
#include <string>
#include <sstream>

size_t get_buf_len(ngx_buf_t* buf);

void combine_chain_buf(ngx_chain_t* bufs, std::string& req_body);

void getArgsMap(const std::string args_str, std::map<std::string, std::string>& args_map);

ngx_int_t make_resp_body(char* buf, size_t len, ngx_chain_t **out, ngx_http_request_t* r);

void make_resp_header(ngx_uint_t stats, ngx_http_request_t *r);

#endif  //PS_SPI_UTILS_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
