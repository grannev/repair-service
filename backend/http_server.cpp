#include "http_server.hpp"

#define HTTPSERVER_IMPL
#define EPOLL
#include "httpserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRONTEND_DIR "../frontend"
#define MAX_PATH_LEN 512

/* Строки httpserver.h не заканчиваются нулём */
static int str_equals(struct http_string_s s, const char *cstr)
{
    return s.len == strlen(cstr) && memcmp(s.buf, cstr, s.len) == 0;
}

static void respond(
    struct http_request_s* request,
    int status,
    const char *content_type,
    const char *body,
    int length
) {
    struct http_response_s *response;

    response = http_response_init();
    http_response_status(response, status);
    http_response_header(response, "Content-Type", content_type);
    http_response_body(response, body, length);
    http_respond(request, response);
}

static void respond_text(
    struct http_request_s *request,
    int status,
    const char *text
) {
    respond(request, status, "text/plain", text, (int)strlen(text));
}

static void redirect(
    struct http_request_s *request,
    const char *location
) {
    struct http_response_s *response;

    response = http_response_init();
    http_response_status(response, 303);
    http_response_header(response, "Location", location);
    http_respond(request, response);
}

static const char *content_type_for(const char *path)
{
    const char *dot;
    const char *ext;
    
    dot = strrchr(path, '.');
    ext = dot ? dot + 1 : "";
    
    if (strcmp(ext, "html") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(ext, "css") == 0)
        return "text/css; charset=utf-8";

    /*
    if (strcmp(ext, "js") == 0)
        return "application/javascript";
    */
    
    if (strcmp(ext, "png") == 0)
        return "image/png";
    if (strcmp(ext, "jpg") == 0)
        return "image/jpeg";
    if (strcmp(ext, "ico") == 0)
        return "image/x-icon";

    return "application/octet-stream";
}

/* GET /something -> ../frontend/something */
static void serve_static(
    struct http_request_s *request,
    struct http_string_s target
) {
    char path[MAX_PATH_LEN];
    char file_path[MAX_PATH_LEN + sizeof(FRONTEND_DIR)];
    char *query;

    FILE *file;
    char *file_content;
    long file_size;
    long file_readed;

    if (target.len >= MAX_PATH_LEN) {
        respond_text(request, 414, "URI too long");
        return;
    }
    memcpy(path, target.buf, target.len);
    path[target.len] = '\0';

    query = strchr(path, '?');
    if (query)
        *query = '\0';
    if (strcmp(path, "/") == 0)
        strcpy(path, "/index.html");
    if (strstr(path, "..")) {
        respond_text(request, 403, "Forbidden");
        return;
    }

    strcpy(file_path, FRONTEND_DIR);
    strcat(file_path, path);

    file = fopen(file_path, "rb");
    if (file == NULL) {
        respond_text(request, 404, "Not found");
        return;
    }
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    file_content = (char*)malloc(file_size > 0 ? file_size : 1);
    file_readed = fread(file_content, 1, file_size, file);
    fclose(file);

    respond(request, 200, content_type_for(path), file_content, file_readed);
    free(file_content); /* http_respond копирует тело в свой буфер */
}

/* POST /api/repair-request */
static void handle_repair_request_form(struct http_request_s *request)
{
    struct http_string_s method;
    struct http_string_s target;
    struct http_string_s body;
    struct http_string_s key;
    struct http_string_s val;
    int iter;

    method = http_request_method(request);
    target = http_request_target(request);
    printf("%.*s %.*s\n", method.len, method.buf, target.len, target.buf);

    iter = 0;
    while (http_request_iterate_headers(request, &key, &val, &iter))
        printf("%.*s: %.*s\n", key.len, key.buf, val.len, val.buf);

    body = http_request_body(request);
    printf("\n%.*s\n\n", body.len, body.buf);
    fflush(stdout);

    redirect(request, "/thanks.html");
}

static void handle_request(struct http_request_s* request)
{
    struct http_string_s method;
    struct http_string_s target;

    method = http_request_method(request);
    target = http_request_target(request);
    if (
        str_equals(method, "POST") && 
        str_equals(target, "/api/repair-request")
    ) {
        handle_repair_request_form(request);
    } else if (str_equals(method, "GET"))
        serve_static(request, target);
    else
        respond_text(request, 405, "Method not allowed");
}

void runserver()
{
    struct http_server_s* server = http_server_init(8080, handle_request);
    http_server_listen(server);
}

