#include "kioku.h"
#include "mongoose.h"
#include "parson.h"

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;
#define BAD_REQUEST "400 Bad Request"

#define rest_respond(connection, codestring, format, ...)               \
  do {                                                                  \
    fprintf(stderr, format "\r\n", __VA_ARGS__);                        \
    mg_printf(connection, "%s", "HTTP/1.1 " codestring "\r\nTransfer-Encoding: chunked\r\n\r\n"); \
    mg_printf_http_chunk(connection, format, __VA_ARGS__);              \
    mg_send_http_chunk(connection, "", 0); /* Send empty chunk, the end of response */ \
  } while(0)

/* Talk to this with `curl localhost:8000/api/v1/sum --data "{\"n1\": YOUR_N1_NUMBER, \"n2\": YOUR_N2_NUMBER}"` */
static void handle_sum_call(struct mg_connection *nc, struct http_message *hm) {
  double result = 0;
  char buf[100] = {0};
  memcpy(buf, hm->body.p,
         sizeof(buf) - 1 < hm->body.len ? sizeof(buf) - 1 : hm->body.len);
  fprintf(stderr, "Data: %s\n", buf);

  /* See if JSON value was provided as data */
  JSON_Value  *root_value = json_parse_string(buf);
  if (root_value == NULL)
  {
    rest_respond(nc, BAD_REQUEST, "{error: \"%s\"}", "JSON value not found");
    json_value_free(root_value);
    return;
  }
  /* See if it was provided as an object */
  JSON_Object *root_object = json_value_get_object(root_value);
  if (root_object == NULL)
  {
    rest_respond(nc, BAD_REQUEST, "{error: \"%s\"}", "JSON root object not found");
    json_value_free(root_value);
    return;
  }
  /* Check members */
  if (!json_object_has_value_of_type(root_object, "n1", JSONNumber))
  {
    rest_respond(nc, BAD_REQUEST, "{error: \"%s\"}", "Expected JSON number 'n1'");
    json_value_free(root_value);
    return;
  }
  if (!json_object_has_value_of_type(root_object, "n2", JSONNumber))
  {
    rest_respond(nc, BAD_REQUEST, "{error: \"%s\"}", "Expected JSON number 'n2'");
    json_value_free(root_value);
    return;
  }

  /* Respond */
  result = json_object_get_number(root_object, "n1") + json_object_get_number(root_object, "n2");
  rest_respond(nc, "200 OK", "{result: %lf}", result);
  json_value_free(root_value);
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (mg_vcmp(&hm->uri, KIOKU_REST_API_PATH "sum") == 0) {
        handle_sum_call(nc, hm); /* Handle RESTful call */
      } else if (mg_vcmp(&hm->uri, "/printcontent") == 0) {
        char buf[100] = {0};
        memcpy(buf, hm->body.p,
               sizeof(buf) - 1 < hm->body.len ? sizeof(buf) - 1 : hm->body.len);
        printf("%s\n", buf);
      } else {
        mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      }
      break;
    default:
      break;
  }
}

int main(int argc, char *argv[]) {
  struct mg_mgr mgr;
  struct mg_connection *nc;
  struct mg_bind_opts bind_opts;
  int i;
  char *cp;
  const char *err_str;
#if MG_ENABLE_SSL
  const char *ssl_cert = NULL;
#endif

  mg_mgr_init(&mgr, NULL);

  /* Use current binary directory as document root */
  if (argc > 0 && ((cp = strrchr(argv[0], DIRSEP)) != NULL)) {
    *cp = '\0';
    s_http_server_opts.document_root = argv[0];
  }

  /* Process command line options to customize HTTP server */
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-D") == 0 && i + 1 < argc) {
      mgr.hexdump_file = argv[++i];
    } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
      s_http_server_opts.document_root = argv[++i];
    } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
      s_http_port = argv[++i];
    } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
      s_http_server_opts.auth_domain = argv[++i];
#if MG_ENABLE_JAVASCRIPT
    } else if (strcmp(argv[i], "-j") == 0 && i + 1 < argc) {
      const char *init_file = argv[++i];
      mg_enable_javascript(&mgr, v7_create(), init_file);
#endif
    } else if (strcmp(argv[i], "-P") == 0 && i + 1 < argc) {
      s_http_server_opts.global_auth_file = argv[++i];
    } else if (strcmp(argv[i], "-A") == 0 && i + 1 < argc) {
      s_http_server_opts.per_directory_auth_file = argv[++i];
    } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
      s_http_server_opts.url_rewrites = argv[++i];
#if MG_ENABLE_HTTP_CGI
    } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
      s_http_server_opts.cgi_interpreter = argv[++i];
#endif
#if MG_ENABLE_SSL
    } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
      ssl_cert = argv[++i];
#endif
    } else {
      fprintf(stderr, "Unknown option: [%s]\n", argv[i]);
      exit(1);
    }
  }

  /* Set HTTP server options */
  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.error_string = &err_str;
#if MG_ENABLE_SSL
  if (ssl_cert != NULL) {
    bind_opts.ssl_cert = ssl_cert;
  }
#endif
  nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
  if (nc == NULL) {
    fprintf(stderr, "Error starting server on port %s: %s\n", s_http_port,
            *bind_opts.error_string);
    exit(1);
  }

  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.enable_directory_listing = "yes";

  printf("Starting RESTful server on port %s, serving %s\n", s_http_port,
         s_http_server_opts.document_root);
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
