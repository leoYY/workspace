/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file ngx_http_aret_recommend.cc
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/03/29 19:06:52
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "ngx_http_aret_recommend.h"
#include "ngx_http_aret_module.h"
#include "utils.h"
#include "aret.h"

extern ngx_module_t ngx_http_aret_module;

char* 
ngx_http_recommend_handler_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf;
    clcf = (ngx_http_core_loc_conf_t*)ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_recommend_req_handler;
    return NGX_CONF_OK;
}

ngx_int_t
ngx_http_recommend_req_handler(ngx_http_request_t *r)
{
    ngx_int_t rc;
    if (!(r->method & NGX_HTTP_GET))            
    {
        return NGX_HTTP_NOT_ALLOWED; 
    }
    ngx_http_aret_process_ctx_t* ctx = (ngx_http_aret_process_ctx_t*)ngx_http_get_module_main_conf(r, ngx_http_aret_module);
    if (ctx == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "process context get failed");
        return NGX_HTTP_INTERNAL_SERVER_ERROR; 
    }
    aret::ReEngine* engine = ctx->engine;
    if (engine == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "get re_engine handler failed.");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    std::vector<std::string> urls;
    std::vector<std::string> titles;
    std::vector<std::string> tags;
    std::string err;
    
    json_object* json_result = json_object_new_array(); 
    if (!engine->GetRecommend("user", 10, &urls, &titles, &tags, &err))
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "get recommend failed. [%s]", err.c_str()); 
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    for (size_t i=0; i<urls.size(); i++)
    {
        std::string url = urls[i];
        std::string title = titles[i];
        std::string tag = tags[i];
        json_object* json_obj = json_object_new_object();
        json_object* json_url = json_object_new_string(url.c_str());
        json_object* json_title = json_object_new_string(title.c_str());
        json_object* json_tag = json_object_new_string(tag.c_str());
        json_object_object_add(json_obj, "url", json_url);
        json_object_object_add(json_obj, "title", json_title);
        json_object_object_add(json_obj, "tag", json_tag);
        json_object_array_add(json_result, json_obj);
    }
    json_object* json_content = json_object_new_object();
    json_object_object_add(json_content, "content", json_result);
    const int MAX_RESP_BUF = 1024 * 10;
    char send_buf[MAX_RESP_BUF];
    int len = snprintf(send_buf, MAX_RESP_BUF, "%s", json_object_to_json_string(json_content));
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "%s", send_buf);
    make_resp_header(NGX_HTTP_OK, r);
    ngx_chain_t *out = NULL;
    rc = make_resp_body(send_buf, len, &out, r);
    if (rc != NGX_OK)
    {
        return rc;
    }
    rc = ngx_http_send_header(r);
    if (rc != NGX_OK)
    {
        return rc; 
    }
    return ngx_http_output_filter(r, out);
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
