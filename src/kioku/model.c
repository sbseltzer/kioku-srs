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
  git_libgit2_init();
  return true;
}

void kioku_model_exit()
{
  git_libgit2_shutdown();
}

bool kioku_model_isrepo(const char *path)
{
  
}

bool kioku_model_init_repo(git_repository **repo_out, const char *path)
{
  bool result = false;
  git_repository_init_options initopts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
  initopts.flags = GIT_REPOSITORY_INIT_MKPATH;
  git_repository_init_ext(repo_out, path, &initopts);
  return result;
}
bool kioku_model_clone_repo(git_repository **repo_out, const char *path, const char *remote_url)
{
  bool result = false;
  git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
  git_clone(repo_out, remote_url, path, &opts);
  return result;
}

bool kioku_deck_open(const char *path)
{
  bool result = false;
  char *fullpath = NULL;
  if (!kioku_model_isrepo(path))
  {
    kioku_model_init_repo(&repo, path);
  }
  int32_t pathlen = kioku_path_concat(NULL, 0, path, ".scheduler");
  if (pathlen <= 0)
  {
    return result;
  }
  fullpath = malloc(pathlen + 1);
  int32_t wrote = kioku_path_concat(fullpath, pathlen + 1, path, ".scheduler");
  if (wrote != pathlen)
  {
    return result;
  }
  result = kioku_filesystem_create(fullpath);
  free(fullpath);
  return result;
}
