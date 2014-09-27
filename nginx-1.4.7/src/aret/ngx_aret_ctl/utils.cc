/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file ./utils.cc
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/03/29 21:30:29
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "utils.h"

size_t get_buf_len(ngx_buf_t* buf)
{
    return buf->last - buf->pos;
}

void combine_chain_buf(ngx_chain_t* bufs, std::string& req_body)
{
    ngx_chain_t* cl;
    for (cl = bufs; cl; cl = cl->next)
    {
        req_body.append((char*)cl->buf->pos, get_buf_len(cl->buf));
    }
    return ;
}

void getArgsMap(const std::string args_str, std::map<std::string, std::string>& args_map)
{
    std::stringstream ss(args_str);
    std::string tmp; 
    while (std::getline(ss, tmp, '&'))
    {
        size_t pos = tmp.find_first_of('=', 0);  
        if (pos == std::string::npos)
        {
            continue; 
        }
        args_map.insert(std::pair<std::string, std::string>(tmp.substr(0, pos), tmp.substr(pos+1)));
    }
    return ;
}

ngx_int_t make_resp_body(char* buf, size_t len, ngx_chain_t **out, ngx_http_request_t* r)
{
    ngx_buf_t *b = NULL;        
    b = (ngx_buf_t*)ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    *out = (ngx_chain_t*)ngx_pcalloc(r->pool, sizeof(ngx_chain_t));
    if (b == NULL || *out == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to allocate resp buf"); 
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    r->headers_out.content_length_n = len;
    (*out)->buf = b;
    (*out)->next = NULL;
    b->pos = (u_char*)buf;
    b->last = b->pos + len;
    b->memory = 1;
    b->last_buf = 1;
    return NGX_OK;
}

void make_resp_header(ngx_uint_t stats, ngx_http_request_t *r)
{
    r->headers_out.content_type.len = sizeof("text/html") - 1;
    r->headers_out.content_type.data = (u_char*)"text/html" ;
    r->headers_out.status = stats;
    r->headers_out.content_length_n = 0;
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
