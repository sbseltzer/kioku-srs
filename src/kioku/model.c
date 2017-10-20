#include "kioku/model.h"
#include "kioku/filesystem.h"

#include "git2.h"
#include "parson.h"
#include "hashmap.h"
#include "utf8.h"
#include "tinydir.h"

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

static size_t file_read_line(char *linebuf, size_t linebuf_size, int32_t linenum, const char *path)
{
  FILE *fp = kioku_filesystem_open(path, "r");
  int ch = EOF;
  if (linenum < 1)
  {
    return 0;
  }
  int i = 1;
  for (ch = fgetc(fp); (i < linenum) && (ch != EOF); ch = fgetc(fp))
  {
    if (ch == '\n')
    {
      i++;
    }
  }
  size_t stored = 0;
  for (ch = fgetc(fp); (stored < linebuf_size-1) && (ch != '\r') && (ch != '\n') && (ch != EOF); ch = fgetc(fp))
  {
    linebuf[stored] = (char) ch;
    stored++;
  }
  linebuf[stored] = '\0';
  fclose(fp);
  return stored;
}

bool srsModel_Card_GetNextID(const char *deck_path, char *card_id_buf, size_t card_id_buf_size)
{
  bool result = false;
  char file_path[kiokuPATH_MAX] = {0};

  /* Get path to the .at file */
  int32_t atlen = kioku_path_concat(file_path, sizeof(file_path), deck_path, ".at");
  result = ((atlen > 0) && (atlen < sizeof(file_path)));
  if (!result)
  {
    return result;
  }

  /* Get content from .at file */
  char atindex[16] = {0};
  result = kioku_filesystem_getcontent(file_path, atindex, sizeof(atindex));
  if (!result)
  {
    return result;
  }
  /* Convert .at content to integer */
  int64_t index = 0;
  char *end = NULL;
  if (sizeof(index) >= sizeof(long long int))
  {
    index = strtoll(atindex, &end, 10);
    result = (index != LLONG_MIN) && (index != LLONG_MAX);
    if (!result)
    {
      srsLOG_ERROR("strtoll produced an out of range value!"kiokuSTRING_LF);
    }
  }
  else if (sizeof(index) >= sizeof(long int))
  {
    index = strtol(atindex, &end, 10);
    /* Check range */
    result = (index != LONG_MIN) && (index != LONG_MAX);
    if (!result)
    {
      srsLOG_ERROR("strtol produced an out of range value!"kiokuSTRING_LF);
    }
  }
  else
  {
    srsLOG_ERROR("Something about integer sizes with strtol/strtoll is incompatible (trying to store %zu or %zu in %zu)!"kiokuSTRING_LF, sizeof(long), sizeof(long long), sizeof(index));
    result = false;
  }
  if (!result)
  {
    return result;
  }
  if (index > INT32_MAX || index < 1)
  {
    srsLOG_ERROR("Line number is out of range for line search."kiokuSTRING_LF);
    return false;
  }
  result = (end != NULL);
  if (!result)
  {
    srsLOG_ERROR("Result of strtol/strtoll was invalid: end == NULL"kiokuSTRING_LF);
    return result;
  }
  result = (index >= 0);
  if (!result)
  {
    srsLOG_ERROR("Result of strtol/strtoll was invalid: index = %d"kiokuSTRING_LF, (int32_t)index);
    return result;
  }
  /* Check the result of integer conversion */
  /** \todo Figure out what counts as a failure state */
  if ((*atindex == '\0') && (*end != '\0'))
  {
    /* Entire string is valid */
    result = true;
  }
  else if ((index == 0) && (end == atindex))
  {
    /* No digits */
    srsLOG_ERROR("Result of strtol/strtoll had no digits"kiokuSTRING_LF);
    result = false;
  }
  else if (*end != '\0')
  {
    /* Stopped at an invalid character */
    srsLOG_ERROR("Result of strtol/strtoll stopped at invalid character %c"kiokuSTRING_LF, *end);
    result = false;
  }

  int32_t schedlen = kioku_path_concat(file_path, sizeof(file_path), deck_path, ".schedule");
  result = ((schedlen > 0) && (schedlen < sizeof(file_path)));
  if (!result)
  {
    srsLOG_ERROR("Schedule file path name length out of range: %d"kiokuSTRING_LF, schedlen);
    return result;
  }
  char linedata[srsMODEL_CARD_ID_MAX] = {0};
  if (card_id_buf_size > sizeof(linedata))
  {
    srsLOG_ERROR("Oversized string: %zu > %zu"kiokuSTRING_LF, card_id_buf_size, sizeof(linedata));
    return false;
  }
  size_t stored = file_read_line(linedata, sizeof(linedata), (int32_t)index, file_path);
  srsLOG_NOTIFY("%ld line: %s"kiokuSTRING_LF, (int32_t)index, linedata);
  if (card_id_buf_size < stored)
  {
    srsLOG_ERROR("Insufficient string size: %zu < %zu"kiokuSTRING_LF, card_id_buf_size, stored);
    return false;
  }
  /* The reading to temp buffer and copying and such is probably redundant at the moment */
  strncpy(card_id_buf, linedata, stored);
  return true;
}

bool kioku_deck_open(const char *path)
{
  bool result = false;
  /* if (!kioku_model_isrepo(path)) */
  /* { */
  /*   kioku_model_init_repo(&repo, path); */
  /* } */
  return result;
}
