#include "kioku/model.h"
#include "kioku/git.h"
#include "kioku/filesystem.h"
#include "kioku/log.h"
#include "kioku/string.h"
#include "kioku/debug.h"
#include "kioku/error.h"

#include "parson.h"
#include "hashmap.h"
#include "utf8.h"
#include "git2.h"

#include <string.h>

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

static const char *srsModel_ROOT_PATH = NULL;

srsRESULT srsModel_SetRoot(const char *path)
{
  if (path == NULL)
  {
    srsLOG_PRINT("Path is NULL - close the underlying repository and nullify the model root.");
    srsGit_Repo_Close();
    free(srsModel_ROOT_PATH);
    srsModel_ROOT_PATH = NULL;
    return srsOK;
  }
  if (!srsDir_Exists(path))
  {
    return srsFAIL;
  }
  if (!srsGit_IsRepo(path))
  {
    return srsFAIL;
  }
  free(srsModel_ROOT_PATH);
  srsModel_ROOT_PATH = NULL;
  srsRESULT result = srsGit_Repo_Open(path);
  if (result == srsOK)
  {
    srsLOG_PRINT("Opened Git repository for the model root at [%s]", path);
    srsModel_ROOT_PATH = strdup(srsGit_Repo_GetCurrent());
    srsASSERT(srsModel_ROOT_PATH != NULL);
    srsLOG_PRINT("Model root is now [%s]", srsModel_ROOT_PATH);
  }
  else
  {
    srsLOG_ERROR("Failed to use [%s] as the model root path - something went wrong trying to open the Git repository.", path);
  }
  return result;
}

const char *srsModel_GetRoot()
{
  return srsModel_ROOT_PATH;
}

#if 0
bool srsModel_IsPathInRoot(const char *path)
{
  if (srsModel_GetRoot() == NULL)
  {
    srsERROR_SET(srsE_API, "Model Root not set!");
    return false;
  }
  if (path == NULL)
  {
    srsERROR_SET(srsE_INPUT, "Path was NULL");
    return false;
  }
  bool result = false;
  const char *root = srsModel_GetRoot();
  git_buf rootbuf = {0};
  size_t rootlen = strlen(root);
  const char *path_to_root = srsDir_PushCWD(root);
  if (path_to_root != NULL)
  {
    int error = git_repository_discover(&rootbuf, path, 0, NULL);
    if (error != 0)
    {
      const git_error *error_struct = giterr_last();
      srsASSERT(error_struct != NULL);
      srsERROR_SET(srsFAIL, error_struct->message);
      result = false;
    }
    else
    {
      srsLOG_PRINT("Discovered root repo: %s", rootbuf.ptr);
      result = strcmp(rootbuf.ptr, root) == 0;
      if (!result)
      {
        srsERROR_SET(srsE_INPUT, "Discovered a root repo, but it was not the model root!");
      }
    }
    srsDir_PopCWD(NULL);
  }
  git_buf_free(&rootbuf); /* returned path data must be freed after use */
  return result;
}
#endif

bool srsModel_Card_GetPath(const char *deck_path, const char *card_id, char *path_out, size_t path_size)
{
  int32_t needed = kioku_path_concat(path_out, path_size, deck_path, card_id);
  const char *ext = ".txt";
  size_t extsize = strlen(ext) + 1;
  if (needed <= 0)
  {
    return false;
  }
  if ((size_t)needed + extsize + 1 > path_size)
  {
    return false;
  }
  strncpy(&path_out[needed], ext, extsize);
  return true;
}

bool srsModel_Card_GetNextID(const char *deck_path, char *card_id_buf, size_t card_id_buf_size)
{
  bool result = false;
  char file_path[srsPATH_MAX] = {0};

  /* Get path to the .at file */
  int32_t atlen = kioku_path_concat(file_path, sizeof(file_path), deck_path, ".at");
  result = ((atlen > 0) && (atlen < sizeof(file_path)));
  if (!result)
  {
    return result;
  }

  /* Get content from .at file */
  char atindex_string[16] = {0};
  result = srsFile_GetContent(file_path, atindex_string, sizeof(atindex_string));
  srsLOG_NOTIFY(".at = %s", atindex_string);
  if (!result)
  {
    return result;
  }

  /* Convert .at content to integer */
  int32_t atindex = 0;
  result = srsString_ToU32(atindex_string, &atindex);
  if (!result)
  {
    return result;
  }
  if (atindex < 1)
  {
    srsLOG_ERROR("Line number is out of range for line search (%d)", atindex);
    return false;
  }

  int32_t schedlen = kioku_path_concat(file_path, sizeof(file_path), deck_path, ".schedule");
  result = ((schedlen > 0) && (schedlen < sizeof(file_path)));
  if (!result)
  {
    srsLOG_ERROR("Schedule file path name length out of range: %d", schedlen);
    return result;
  }
  char linedata[srsMODEL_CARD_ID_MAX] = {0};
  int32_t linelen = srsFile_ReadLineByNumber(file_path, (uint32_t)atindex, linedata, sizeof(linedata));
  if (linelen < 0)
  {
    srsLOG_ERROR("%d line invalid", atindex);
  }
  srsLOG_NOTIFY("%ld line: %s", atindex, linedata);
  if (card_id_buf_size < linelen + 1)
  {
    srsLOG_ERROR("Insufficient string size: %zu < %zu", card_id_buf_size, linelen);
    return false;
  }
  /** @todo Should I check srsMODEL_CARD_ID_MAX agains the card_id_buf_size? Does it make sense to let the user choose something larger? */
  /* The reading to temp buffer and copying and such is probably redundant at the moment */
  result = (linelen > 0);
  if (result)
  {
    strncpy(card_id_buf, linedata, linelen + 1);
  }
  return result;
}

