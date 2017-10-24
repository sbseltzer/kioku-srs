#include "kioku.h"
#include "mongoose.h"
#include "parson.h"
/* #include "json.h" */

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;
static bool kill_me_now = false;
#define HTTP_BAD_REQUEST "400 Bad Request"
#define HTTP_INTERNAL_ERROR "500 Internal Server Error"
#define HTTP_OK "200 OK"

#define rest_respond(connection, codestring, format, ...)               \
  do {                                                                  \
    fprintf(stderr, format "\r\n", __VA_ARGS__);                        \
    mg_printf(connection, "HTTP/1.1 %s\r\nTransfer-Encoding: chunked\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n\r\n", codestring); \
    mg_printf_http_chunk(connection, format, __VA_ARGS__);              \
    mg_send_http_chunk(connection, "", 0); /* Send empty chunk, the end of response */ \
  } while(0)

static bool parse_request(struct http_message *hm, JSON_Value **root_value, JSON_Object **root_object, const char **error_msg)
{
  /* See if JSON value was provided as data */
  *root_value = json_parse_string(hm->body.p);
  *root_object = NULL;
  *error_msg = NULL;
  if (*root_value == NULL)
  {
    *error_msg = "JSON value not found";
    return false;
  }
  /* See if it was provided as an object */
  *root_object = json_value_get_object(*root_value);
  if (*root_object == NULL)
  {
    *error_msg = "JSON root object not found";
    return false;
  }
  return true;
}
static bool precheck_response(JSON_Value **root_value, JSON_Object **root_object, const char *error_msg)
{
  if (*root_value == NULL)
  {
    /* If it failed too early, initialize a fresh JSON object and set the error accordingly */
    *root_value = json_value_init_object();
    *root_object = json_value_get_object(*root_value);
    json_object_set_string(*root_object, "error", error_msg);
    return false;
  }
  return true;
}

static void handle_exit_call(struct mg_connection *nc, struct http_message *hm)
{
  kill_me_now = true;
  kLOG_WRITE("%s", "Set kill flag");
  rest_respond(nc, HTTP_OK, "%s", "{\"result\":\"OK\"}");
}

static void handle_GetNextCard(struct mg_connection *nc, struct http_message *hm)
{
  const char *error_msg = NULL;
  const char *codestring = HTTP_OK;
  JSON_Value *root_value = NULL;
  JSON_Object *root_object = NULL;
  char *serialized_string = NULL;
  const char *deck_path = NULL;
  if (parse_request(hm, &root_value, &root_object, &error_msg))
  {
    deck_path = json_object_get_string(root_object, "deck");
  }
  if (deck_path == NULL)
  {
    goto respond;
  }
  if (!kioku_filesystem_isdir(deck_path))
  {
    goto respond;
  }
  char card_id[srsMODEL_CARD_ID_MAX] = {0};
  if (srsModel_Card_GetNextID("deck-test/", card_id, sizeof(card_id)))
  {
    root_value = json_value_init_object();
    root_object = json_value_get_object(root_value);
    JSON_Value *buttons_value = NULL;
    JSON_Array *buttons = NULL;
    JSON_Value *button_value = NULL;
    JSON_Object *button = NULL;
    {
      json_object_set_string(root_object, "id", card_id);
      json_object_set_string(root_object, "front", "front text");
      json_object_set_string(root_object, "back", "front text<br>back text which included the front text");
      buttons_value = json_value_init_array();
      buttons = json_value_get_array(buttons_value);
      json_object_set_value(root_object, "buttons", buttons_value);
      {
        button_value = json_value_init_object();
        button = json_value_get_object(button_value);
        json_object_set_string(button, "title", "Again");
        json_object_set_number(button, "grade", 1);
        json_array_append_value(buttons, button_value);

        button_value = json_value_init_object();
        button = json_value_get_object(button_value);
        json_object_set_string(button, "title", "Good");
        json_object_set_number(button, "grade", 3);
        json_array_append_value(buttons, button_value);

        button_value = json_value_init_object();
        button = json_value_get_object(button_value);
        json_object_set_string(button, "title", "Easy");
        json_object_set_number(button, "grade", 4);
        json_array_append_value(buttons, button_value);
      }
    }
    serialized_string = json_serialize_to_string_pretty(root_value);
  }
respond:
  if (deck_path == NULL)
  {
    codestring = HTTP_BAD_REQUEST;
    rest_respond(nc, codestring, "%s", "{\"error\":\"No deck specified!\"}");
  }
  else if (!kioku_filesystem_isdir(deck_path))
  {
    codestring = HTTP_BAD_REQUEST;
    rest_respond(nc, codestring, "%s", "{\"error\":\"Specified deck does not exist!\"}");
  }
  else if (serialized_string == NULL)
  {
    codestring = HTTP_INTERNAL_ERROR;
    rest_respond(nc, codestring, "%s", "{\"error\":\"failed to construct response\"}");
  }
  else
  {
    rest_respond(nc, codestring, "%s", serialized_string);
    json_free_serialized_string(serialized_string);
  }
  json_value_free(root_value);
}

static void handle_GetVersion(struct mg_connection *nc, struct http_message *hm)
{
  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);
  char *serialized_string = NULL;
  json_object_set_string(root_object, "version", KIOKU_VERSION);
  serialized_string = json_serialize_to_string_pretty(root_value);
  const char *codestring = HTTP_OK;
  if (serialized_string == NULL)
  {
    codestring = HTTP_INTERNAL_ERROR;
    serialized_string = "{\"error\":\"failed to construct response\"}";
  }
  rest_respond(nc, codestring, "%s", serialized_string);
  json_free_serialized_string(serialized_string);
  json_value_free(root_value);
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
    {
      if (mg_vcmp(&hm->uri, KIOKU_REST_API_PATH "exit") == 0)
      {
        handle_exit_call(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, KIOKU_REST_API_PATH "version") == 0)
      {
        handle_GetVersion(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, KIOKU_REST_API_PATH "card/next") == 0)
      {
        handle_GetNextCard(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, "/printcontent") == 0)
      {
        char buf[100] = {0};
        memcpy(buf, hm->body.p,
               sizeof(buf) - 1 < hm->body.len ? sizeof(buf) - 1 : hm->body.len);
        printf("%s\n", buf);
      } else {
        mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      }
      break;
    }
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
  while (!kill_me_now)
  {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  /* Cleanup logger resources */
  kioku_log_Exit();

  return 0;
}
