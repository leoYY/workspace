/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file ngx_http_aret_record.cc
 * @author yuanyi03(yuanyi03@baidu.com)
 * @date 2014/03/29 19:15:29
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "ngx_http_aret_record.h"
#include "utils.h"
#include "ngx_http_aret_module.h"

extern ngx_module_t ngx_http_aret_module;

char* ngx_http_record_handler_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf;
    clcf = (ngx_http_core_loc_conf_t*)ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_record_req_handler;
    return NGX_CONF_OK;
}

ngx_int_t 
ngx_http_record_req_handler(ngx_http_request_t *r)
{
    ngx_int_t rc;
    if (!(r->method & NGX_HTTP_POST|NGX_HTTP_GET))
    {
        return NGX_HTTP_NOT_ALLOWED; 
    }

    if (r->method & NGX_HTTP_GET)
    {
        // do get request
        rc = ngx_http_discard_request_body(r);
        if (rc != NGX_OK)
        {
            return rc; 
        }
        using namespace teraeasy;
        ngx_http_aret_process_ctx_t *ctx = NULL;             
        ctx = (ngx_http_aret_process_ctx_t*) ngx_http_get_module_main_conf(r, ngx_http_aret_module);
        Table* user_table = ctx->user_table;
        if (user_table == NULL)
        {
            return NGX_HTTP_INTERNAL_SERVER_ERROR; 
        }            
        std::string args_str((char*)r->args.data, r->args.len);
        std::map<std::string, std::string> argsM;
        getArgsMap(args_str, argsM);
        Column c;
        c[0] = argsM["key"];
        Record record;
        record["url"] = c;
        if (!user_table->Write(argsM["imdi"], record))
        {
            make_resp_header(NGX_HTTP_BAD_REQUEST, r);
        }
        else
        {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "add suc");
            make_resp_header(NGX_HTTP_OK, r);
        }
        rc = ngx_http_send_header(r);
        if (rc != NGX_OK)
        {
            return rc; 
        }
        ngx_chain_t *out = NULL;
        make_resp_body(NULL, 0, &out, r);
        return ngx_http_output_filter(r, out);
    }


    // do post
    rc = ngx_http_read_client_request_body(r, ngx_http_aret_record_read_body_handler);
    return rc;
}

void 
ngx_http_aret_record_read_body_handler(ngx_http_request_t *r)
{
    ngx_int_t rc;
    if (!r->request_body)
        return ngx_http_finalize_request(r, NGX_ERROR);
    size_t body_len = r->headers_in.content_length_n;
    body_len = body_len;
    //char* body_buf = (char*) ngx_pcalloc(r->pool, sizeof(char) * body_len);
    //if (body_buf == NULL)
    //{
    //    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "body buf calloc failed.");
    //    return ngx_http_finalize_request(r, NGX_ERROR);
    //}
    ngx_http_aret_process_ctx_t *ctx = NULL;             
    ctx = (ngx_http_aret_process_ctx_t*) ngx_http_get_module_main_conf(r, ngx_http_aret_module);
    aret::ReEngine* engine = ctx->engine;
    if (engine == NULL)
    {
        rc = NGX_ERROR;
        return ngx_http_finalize_request(r, rc);
    }

    std::string url;
    std::string body_buf;
    std::string html;
    std::string err;
    std::string user; 
    std::vector<std::string> content;
    std::map<std::string, std::string> args_map;

    combine_chain_buf(r->request_body->bufs, body_buf); 
    getArgsMap(body_buf, args_map);

    url = args_map["url"];
    user = args_map["imei"];
    content.push_back(url);
    engine->RecordActions(user.c_str(), aret::AT_VIEW, content, &err);
    if (url == "" || !engine->GetHtml(url, &html))
    {
        rc = NGX_ERROR;
        return ngx_http_finalize_request(r, rc);
    }

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "%s", body_buf.c_str());

    rc = NGX_OK;
    make_resp_header(NGX_HTTP_OK, r);
    ngx_chain_t* out = NULL;
    rc = make_resp_body((char*)html.c_str(), html.size(), &out, r);
    if (rc != NGX_OK)
    {
        return ngx_http_finalize_request(r, rc);
    }
    rc = ngx_http_send_header(r);
    if (rc != NGX_OK)
    {
        return ngx_http_finalize_request(r, rc); 
    }
    return ngx_http_finalize_request(r, ngx_http_output_filter(r, out));
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
