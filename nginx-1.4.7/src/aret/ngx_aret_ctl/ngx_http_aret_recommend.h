/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file ngx_http_aret_recommend.h
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/03/29 19:03:37
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef PS_SPI_NGX_HTTP_ARET_RECOMMEND_H
#define PS_SPI_NGX_HTTP_ARET_RECOMMEND_H

extern "C" {
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
}

char* ngx_http_recommend_handler_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
ngx_int_t ngx_http_recommend_req_handler(ngx_http_request_t *r);

#endif  //PS_SPI_NGX_HTTP_ARET_RECOMMEND_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
