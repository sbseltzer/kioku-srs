#include "kioku/model.h"

#include "git2.h"
#include "parson.h"
#include "hashmap.h"
#include "utf8.h"
#include "tinydir.h"

static git_repository *repo = NULL;

static bool kioku_model_load_userlist()
{
  JSON_Value *root_value = json_parse_file(KIOKU_MODEL_USERLIST_NAME);
  if (root_value == NULL)
  {
    /* Create default user list */
    JSON_Value *user_root_value = json_parse_string("{\"name\":\"Default\",\"path\":\"\"}");
    JSON_Object *user_object = json_value_get_object(user_root_value);
    json_object_set_string(user_object, "path", "./users/Default/");
    root_value = json_value_init_array();
    json_array_append_value(json_value_get_array(root_value), user_root_value);
    json_serialize_to_file_pretty(root_value, KIOKU_MODEL_USERLIST_NAME);
  }
  return true;
}

bool kioku_model_init(uint32_t argc, char **argv)
{
  /* git_libgit2_init(); */
  return true;
}

void kioku_model_exit()
{
  /* git_libgit2_shutdown(); */
}
