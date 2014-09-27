/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file ngx_http_aret_module.cc
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/03/29 17:38:28
 * @version $Revision$ 
 * @brief 
 *  
 **/

extern "C" {
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
}

#include "ngx_http_aret_record.h"
#include "aret.h"
#include "ngx_http_aret_module.h"
#include "ngx_http_aret_recommend.h"

#define DEF_HANDER( item ) \
{ngx_string(#item),\
    NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,\
    ngx_http_ ##item## _handler_set, \
    NGX_HTTP_LOC_CONF_OFFSET, \
    0, \
    NULL}

// create srv conf
static void *ngx_http_aret_create_srv_conf(ngx_conf_t* cf);
// create main conf and mem
static void *ngx_http_aret_create_main_conf(ngx_conf_t* cf);

// init process
static ngx_int_t ngx_http_aret_process_init(ngx_cycle_t *cycle);
// un init process
static void ngx_http_aret_process_uninit(ngx_cycle_t *cycle);

static ngx_command_t ngx_http_aret_commands[] = {
    { ngx_string("web_table"),
        NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,     
        NGX_HTTP_MAIN_CONF_OFFSET,
        offsetof(ngx_http_aret_process_ctx_t, web_table_name),
        NULL},
    { ngx_string("tag_table"),
        NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,     
        NGX_HTTP_MAIN_CONF_OFFSET,
        offsetof(ngx_http_aret_process_ctx_t, tag_table_name),
        NULL},
    { ngx_string("user_table"),
        NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,     
        NGX_HTTP_MAIN_CONF_OFFSET,
        offsetof(ngx_http_aret_process_ctx_t, user_table_name),
        NULL},
    DEF_HANDER(record),
    DEF_HANDER(recommend)
};

static ngx_http_module_t ngx_http_aret_module_ctx = {
    NULL,   /* preconfiguration */
    NULL,   /* postconfiguration */
    ngx_http_aret_create_main_conf,   /* create main configuration */
    NULL,   /* init main configuration */
    ngx_http_aret_create_srv_conf,   /* create server configuration */
    NULL,   /* merge server configuration */
    NULL,   /* create location configration */
    NULL,   /* merge location configration */
};

ngx_module_t ngx_http_aret_module = {
    NGX_MODULE_V1,
    &ngx_http_aret_module_ctx, /* module context */
    ngx_http_aret_commands, /* module directives */
    NGX_HTTP_MODULE, /* module type */
    NULL, /* init master */
    NULL, /* init module */
    &ngx_http_aret_process_init, /* init process */
    NULL, /* init thread */
    NULL, /* exit thread */
    &ngx_http_aret_process_uninit, /* exit process */
    NULL, /* exit master */
    NGX_MODULE_V1_PADDING
};

static void *ngx_http_aret_create_main_conf(ngx_conf_t* cf)
{
    ngx_http_aret_process_ctx_t* conf;
    conf = (ngx_http_aret_process_ctx_t*) ngx_pcalloc(cf->pool, sizeof(ngx_http_aret_process_ctx_t));
    if (conf == NULL)
    {
        return NGX_CONF_ERROR; 
    }
    return conf;
}

static void *ngx_http_aret_create_srv_conf(ngx_conf_t* cf)
{
    ngx_http_aret_db_conf_t *conf;
    conf = (ngx_http_aret_db_conf_t*)ngx_pcalloc(cf->pool, sizeof(ngx_http_aret_db_conf_t));
    if (conf == NULL)
    {
        return NGX_CONF_ERROR; 
    }
    return conf;
}

static void ngx_http_aret_process_uninit(ngx_cycle_t *cycle)
{
    ngx_http_aret_process_ctx_t* proc_ctx;
    proc_ctx = (ngx_http_aret_process_ctx_t*) ngx_http_cycle_get_module_main_conf(cycle, ngx_http_aret_module);
    if (proc_ctx == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "get main conf failed."); 
    }
    // delete table handler
    return ;    
}

static ngx_int_t ngx_http_aret_process_init(ngx_cycle_t *cycle)
{
    ngx_http_aret_process_ctx_t* proc_ctx;
    proc_ctx = (ngx_http_aret_process_ctx_t*) ngx_http_cycle_get_module_main_conf(cycle, ngx_http_aret_module);
    if (proc_ctx == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "get main conf failed."); 
        return NGX_ERROR;
    }

    if (proc_ctx->web_table_name.len != 0)
    {
        proc_ctx->web_table = teraeasy::OpenTable((char*)proc_ctx->web_table_name.data); 
        if (proc_ctx->web_table == NULL)
        {
            ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "open table web_table[%s] failed",proc_ctx->web_table_name.data);
            return NGX_ERROR;
        }
        ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "open table web_table[%s] suc", proc_ctx->web_table_name.data);
    }

    if (proc_ctx->user_table_name.len != 0)
    {
        proc_ctx->user_table = teraeasy::OpenTable((char*)proc_ctx->user_table_name.data); 
        if (proc_ctx->user_table == NULL)
        {
            ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "open table user_table[%s] failed",proc_ctx->user_table_name.data);
            return NGX_ERROR;
        }
        ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "open table user_table[%s] suc", proc_ctx->user_table_name.data);
    }

    if (proc_ctx->tag_table_name.len != 0)
    {
        proc_ctx->tag_table = teraeasy::OpenTable((char*)proc_ctx->tag_table_name.data); 
        if (proc_ctx->tag_table == NULL)
        {
            ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "open table tag_table[%s] failed",proc_ctx->tag_table_name.data);
            return NGX_ERROR;
        }
        ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "open table tag_table[%s] suc", proc_ctx->tag_table_name.data);
    }

    proc_ctx->engine = new aret::ReEngine();
    proc_ctx->engine->Init((char*)proc_ctx->web_table_name.data, 
            (char*)proc_ctx->tag_table_name.data,
            (char*)proc_ctx->user_table_name.data);
    return NGX_OK;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
