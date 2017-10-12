#include "kioku/types.h"
#include "kioku/filesystem.h"
#include "kioku/log.h"
#include "tinydir.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef kiokuOS_WINDOWS
#include <direct.h>
#include <io.h>
/* Microsoft (correctly) refuses to recognize strdup as being ISO C compliant,
   despite being part of POSIX, and recommends its own standard-compliant name. */
#define strdup _strdup
#define rmdir _rmdir
#define getcwd _getcwd
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

/* \todo Perhaps have a method called by an init that dynamically finds a "true" max path length */

static FILE *kioku_filesystem_open(const char *path, const char *mode)
{
#ifdef kiokuOS_WINDOWS
  FILE *fp = NULL;
  fopen_s(&fp, path, mode);
#else
  FILE *fp = fopen(path, mode);
#endif
  return fp;
}

/** \todo It may be a good idea to have a max path length and use a strnlen-like method. */

size_t kioku_path_getfull(const char *relative, char *path_out, size_t nbytes)
{
  size_t result = 0;
  char *path = NULL;
  /* \todo mitigate max path length using the getcwd and chdir method described below for all platforms */
  /* QUOTH THE SPEC: http://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
     If the pathname obtained from getcwd() is longer than {PATH_MAX} bytes, it could produce an [ENAMETOOLONG] error if passed to chdir().
     Therefore, in order to return to that directory it may be necessary to break the pathname into sections shorter than {PATH_MAX} bytes and call chdir() on each section in turn (the first section being an absolute pathname and subsequent sections being relative pathnames).
     A simpler way to handle saving and restoring the working directory when it may be deeper than {PATH_MAX} bytes in the file hierarchy is to use a file descriptor and fchdir(), rather than getcwd() and chdir().
     However, the two methods do have some differences. The fchdir() approach causes the program to restore a working directory even if it has been renamed in the meantime, whereas the chdir() approach restores to a directory with the same name as the original, even if the directories were renamed in the meantime.
     Since the fchdir() approach does not access parent directories, it can succeed when getcwd() would fail due to permissions problems.
     In applications conforming to earlier versions of this standard, it was not possible to use the fchdir() approach when the working directory is searchable but not readable, as the only way to open a directory was with O_RDONLY, whereas the getcwd() approach can succeed in this case. */
  char cwd[kiokuPATH_MAX+1] = {0};
  path = getcwd(cwd, nbytes);
  if (path == cwd)
  {
    uint32_t i = 0;
    while (path[i] != kiokuCHAR_NULL)
    {
      printf("Examining %c" kiokuSTRING_LF, path[i]);
      fflush(stdout);
      if (path[i] == '\\')
      {
        path[i] = '/';
      }
      i++;
    }
    while (*path == '.')
    {
      path++;
    }
    int32_t needed = kioku_path_concat(path_out, nbytes, path, relative);
    if (needed > 0)
    {
      result = (size_t) needed;
    }
  }
#if 0
#if kiokuOS_WINDOWS
  path = _fullpath(path_out, relative, nbytes);
#else
#endif
#endif
  return result;
}
void kioku_path_trimpoints(const char *path, size_t *start, size_t *end)
{
  size_t path_start = 0;
  size_t path_end = 0;
  if (start == NULL || end == NULL)
  {
    return;
  }
  if (path != NULL)
  {
    path_end = strlen(path);
    /* Index of strlen is at the null terminator, but we want to end at the last valid char. */
    if (path_end > 0)
    {
      path_end--;
    }
    /* Strip extra slashes on end of path */
    while ((path_end > path_start) && (path[path_end] == '/'))
    {
      path_end--;
    }
    /* Strip extra slashes at start of path */
    while ((path_start < path_end) && (path[path_start] == '/'))
    {
      path_start++;
    }
    /* If we stripped ALL slashes, make sure there's one leading on left */
    if (path_start > 0)
    {
      path_start--;
    }
  }
  *start = path_start;
  *end = path_end;
}

int32_t kioku_path_concat(char *dest, size_t destsize, const char *path1, const char *path2)
{
  /* Don't let the user use this as an append or anything dangerous like that. */
  if (dest == path1 || dest == path2)
  {
    return -1;
  }
  size_t path1_start, path1_end, path2_start, path2_end;
  kioku_path_trimpoints(path1, &path1_start, &path1_end);
  kioku_path_trimpoints(path2, &path2_start, &path2_end);
  /* The right hand portion should not have a leading slash */
  if (path2[path2_start] == '/')
  {
    path2_start++;
  }
  uint32_t wrotelen = 0;
  uint32_t neededlen = 0;
  size_t pi = 0;
  size_t max_index = (destsize > 0) ? destsize - 1 : 0;
  /* Tack on path1 */
  for (pi = path1_start; (pi <= path1_end) && (path1[pi] != '\0'); pi++)
  {
    if ((dest != NULL) && (wrotelen < max_index))
    {
      dest[wrotelen] = path1[pi];
      wrotelen++;
    }
    neededlen++;
  }
  /* Add separator iff path1 had content that didn't end with a separator and path2 non-empty */
  if ((neededlen > 0) && (path1[path1_end] != '/') && (path2[path2_start] != '\0'))
  {
    if ((dest != NULL) && (wrotelen < max_index))
    {
      dest[wrotelen] = '/';
      wrotelen++;
    }
    neededlen++;
  }
  /* Tack on path2 */
  for (pi = path2_start; (pi <= path2_end) && (path2[pi] != 0); pi++)
  {
    if ((dest != NULL) && (wrotelen < max_index))
    {
      dest[wrotelen] = path2[pi];
      wrotelen++;
    }
    neededlen++;
  }
  /* Sanity check */
  assert(wrotelen <= max_index);
  assert(wrotelen <= neededlen);
  /* Null terminate */
  /* NOTE: We need to make sure the destsize isn't zero before we try writing to it since max_index alone might deceive us */
  if ((dest != NULL) && (wrotelen <= max_index) && (destsize > 0))
  {
    dest[wrotelen] = '\0';
    /* One more sanity check */
    assert(wrotelen == strlen(dest));
  }
  /* Return number of bytes that were needed, minus the null terminator (i.e. string length) */
  assert(neededlen < INT32_MAX);
  return neededlen;
}

int32_t kioku_path_up_index(const char *path, int32_t start_index)
{
  int32_t result = -1;
  if (path == NULL)
  {
    return result;
  }
  size_t pathlen = strlen(path);
  if (pathlen == 0)
  {
    return result;
  }
  assert((int32_t)pathlen < INT32_MAX);
  /* March backward til it hits a separator. */
  for (result = (int32_t)pathlen - 1; (result >= 0) && (path[result] != '/'); result--);
  return result;
}

bool kioku_filesystem_create(const char *path)
{
  if (path == NULL)
  {
    return false;
  }
  if (kioku_filesystem_exists(path))
  {
    return false;
  }
  /* Do not allow creating an empty directory */
  if (path[strlen(path)] == '/')
  {
    return false;
  }
  char *dupedpath = strdup(path);
  char *found = dupedpath;
  while (found != NULL)
  {
    found = strchr(found, '/');
    if (found == NULL)
    {
      break;
    }
    if (kioku_filesystem_exists(dupedpath))
    {
      continue;
    }
    char oldval = found[1];
    found[1] = 0;
#ifdef kiokuOS_WINDOWS
    bool ok = _mkdir(dupedpath) == 0;
#else
    bool ok = mkdir(dupedpath, 0700) == 0;
#endif
    /* We don't do anything with ok for now. If we break the loop here we won't get to attempt all the directories we need to. */
    found[1] = oldval;
    found++;
  }
  free(dupedpath);
  /* Creating file */

  FILE *fp = kioku_filesystem_open(path, "w");
  return (fp != NULL) && (fclose(fp) == 0);
}

bool kioku_filesystem_rename(const char *path, const char *newpath)
{
  bool result = false;
  if (path == NULL || newpath == NULL)
  {
    return result;
  }
  result = rename(path, newpath) == 0;
  return result;
}

#define KIOKU_DIR_MAX_DEPTH 128
bool kioku_filesystem_delete(const char *path)
{
  bool result = false;
  if (path == NULL)
  {
    return result;
  }
  if (!kioku_filesystem_exists(path))
  {
    return result;
  }
  if (kioku_filesystem_isdir(path))
  {
    result = rmdir(path) == 0;
  }
  else
  {
    result = remove(path) == 0;
  }
  return result;
}

bool kioku_filesystem_exists(const char *path)
{
  bool result = false;
  if (path == NULL)
  {
    return result;
  }
#ifdef kiokuOS_WINDOWS
  /* https://stackoverflow.com/a/6218957 */
  result = GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
#else
  result = access(path, R_OK) == 0;
#endif
  return result;
}

bool kioku_filesystem_isdir(const char *path)
{
  bool result = false;
  if (path == NULL)
  {
    return result;
  }
  if (!kioku_filesystem_exists(path))
  {
    return result;
  }
#ifdef kiokuOS_WINDOWS
  /* https://stackoverflow.com/a/6218957 */
  result = (GetFileAttributes(path) & FILE_ATTRIBUTE_DIRECTORY);
#else
  /* https://stackoverflow.com/a/4553053 */
  struct stat statbuf;
  if (stat(path, &statbuf) == 0)
  {
    result = S_ISDIR(statbuf.st_mode);
  }
#endif
  return result;
}

int32_t kioku_filesystem_getlen(const char *filepath)
{
  int32_t result = -1;
  if (filepath == NULL)
  {
    return result;
  }
  if (!kioku_filesystem_exists(filepath))
  {
    return result;
  }
  if (kioku_filesystem_isdir(filepath))
  {
    return result;
  }
  FILE *fp = kioku_filesystem_open(filepath, "r");
  if (fp != NULL)
  {
    fseek(fp, 0, SEEK_END);
    result = ftell(fp);
  }
  fclose(fp);
  return result;
}

bool kioku_filesystem_setcontent(const char *filepath, const char *content)
{
  bool result = false;
  if (filepath == NULL)
  {
    return result;
  }
  if (content == NULL)
  {
    return result;
  }
  if (!kioku_filesystem_exists(filepath))
  {
    return result;
  }
  if (kioku_filesystem_isdir(filepath))
  {
    return result;
  }
  FILE *fp = kioku_filesystem_open(filepath, "w");
  if (fp == NULL)
  {
    return result;
  }
  result = fputs(content, fp) >= 0;
  fclose(fp);
  return result;
}

bool kioku_filesystem_getcontent(const char *filepath, char *content_out, size_t count)
{
  bool result = false;
  if (filepath == NULL)
  {
    return result;
  }
  if (content_out == NULL)
  {
    return result;
  }
  if (count == 0)
  {
    return result;
  }
  if (!kioku_filesystem_exists(filepath))
  {
    return result;
  }
  if (kioku_filesystem_isdir(filepath))
  {
    return result;
  }
  FILE *fp = kioku_filesystem_open(filepath, "r");
  if (fp == NULL)
  {
    return result;
  }
  char *str = fgets(content_out, count, fp);
  result = str == content_out;
  fclose(fp);
  return result;
}
