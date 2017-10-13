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
/** \todo implement a relative path resolver that eliminates . and .. - https://linux.die.net/man/3/realpath */

void kioku_path_replace_separators(char *path, size_t nbytes)
{
  size_t i = 0;
  if ((path != NULL) && (nbytes > 0))
  {
    while ((path[i] != kiokuCHAR_NULL) && (i < nbytes))
    {
      if (path[i] == '\\')
      {
        path[i] = '/';
      }
      i++;
    }
  }
}

void kioku_path_resolve_relative(char *path, int32_t nbytes)
{
  size_t i = 0;
  size_t offset = 0;
  if ((path != NULL) && (nbytes > 0))
  {
    while ((path[i] != kiokuCHAR_NULL) && (i < nbytes))
    {
      if (path[i] == '.')
      {
        /* Starting with a relative dir simply returns - it's difficult (or impossible) to know what the correct conversion is */
        if (i == 0)
        {
          break;
        }
        /* Okay, so we're at a relative "go-up" path and it's not at the very start of the string */
        if (path[i+1] == '.')
        {
          /* \todo Handle case of invalid "...*" */
          /* Go up one directory to eliminate the .. */
          int32_t up = kioku_path_up_index(path, i);
          /* Because we started at a finite int32_t, we should never end up in a situation where up_index returns less than -1 */
          assert(up >= -1);
          /* If we've hit no separators, it means we're at the beginning of the string, in which case which case can't resolve this path any further. */
          if (up == -1)
          {
            
          }
          /* If we've hit the first char of the string, it means it's a root path, in which case we can't resolve any further */
          else if (up == 0)
          {
            
          }
          if ((path[up] != '/') && (path[up] != '\\'))
          {
            
          }
          /* Go up another directory to eliminate the parent */
          up = kioku_path_up_index(path, up);
          i++; /* Increment forward so we know where to move back from */
        }
        else
        {
          /* Simple eliminate this part of the path. */

        }
      }
      i++;
    }
  }
}

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

  /* Right now this is a troublesome implementation, as it allocates max path size even if the user specifies something lower. Unfortunately, getcwd doesn't provide a mechanism for knowing how much space is needed. */
  /* getcwd is apparently deprecated for portability reasons, as noted here: https://linux.die.net/man/3/getcwd - supposedly the more reliable option to PATH_MAX is https://linux.die.net/man/3/pathconf on POSIX platforms, especially since PATH_MAX is not required to be a compile-time constant. */
  /* Some platforms implement getcwd to malloc space when passed a null pointer. Both the Windows CRT _getcwd and the extension to the POSIX.1-2001 standard, Linux (libc4, libc5, glibc) implement it in this way. */
  /* The WINAPI GetCurrentDirectory does return the required length if not long enough, which is basically what we want here. It's not threadsafe, but currenly Kioku does not attempt to be. https://msdn.microsoft.com/en-us/library/windows/desktop/aa364934(v=vs.85).aspx */
  char cwd[kiokuPATH_MAX+1] = {0};
  path = getcwd(cwd, nbytes);
  if (path == cwd)
  {
    kioku_path_replace_separators(path, sizeof(cwd));
    int32_t needed = kioku_path_concat(path_out, nbytes, path, relative);
    if (needed > 0)
    {
      result = (size_t) needed;
    }
    /* \todo resolve the relative paths to mimic the functionality of realpath and _fullpath */
    /* result = kioku_path_resolve_relative(path_out, result); */
  }
#if 0
  /* Consider using realpath and _fullpath */
#if kiokuOS_WINDOWS
  path = _fullpath(path_out, relative, nbytes);
#elif _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
  char full[kiokuPATH_MAX+1] = {0};
  realpath(path, full);
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
  if (*path == kiokuCHAR_NULL)
  {
    return result;
  }
  /* \todo Perhaps allow user to specify a string length, though start_index is probably sufficient for that purpose. */

  /* A start index of 1 or less would always yield -1, but we'll enforce -1 as being the indicator for using the path string length. This is because a user could use this in a loop where they feed the return value of this function into itself, and not having a consistent number to check against could be confusing/dangerous. */
  /* Example:       for (i = strlen(path); i > -1; i = kioku_path_up_index(path, i))
     As opposed to: for (i = -1; i != -1; i = kioku_path_up_index(path, i))
     If `i` starts at less than -1 and we assume it means "use path length", then in the unlikely case of a 32bit int not being able to contain a large size_t, the user could end up in an infinite loop by feeding the result of this function into itself.
     */
  if (start_index < -1)
  {
    return result;
  }
  else if (start_index == -1)
  {
    size_t length = strlen(path);
    start_index = (int32_t)length;
  }
  /* Bounds check
     These could only be violated if size_t is longer than the max int32 length and paths are able to exceed that.
     Or if the path is not null terminated, strlen could return an erroneously long value, which could overflow start_index into negative bounds. */
  /* \todo Test this */
  if (start_index < 0 || start_index >= INT32_MAX)
  {
    result = INT32_MIN;
    return result;
  }
  /* First eliminate dangling NULLs (if any) */
  /* \todo Test this */
  result = start_index;
  while ((result >= 0) && (path[result] == kiokuCHAR_NULL))
  {
    result--;
  }
  /* Sanity check: If it hits the beginning of the string, it was apparently empty - this case should be captured above. */
  assert(result >= 0);
  /* If the path ends with a separator, we need to march back to the first non-separator before traversing to the parent. */
  if (kiokuCHAR_ISDIRSEP(path[result]))
  {
    /* March back to first non-separator */
    while ((result >= 0) && kiokuCHAR_ISDIRSEP(path[result]))
    {
      result--;
    }
  }
  /* March back to first separator to eliminate the file/dir we're on in the path. */
  while ((result >= 0) && !kiokuCHAR_ISDIRSEP(path[result]))
  {
    result--;
  }
  /* March back to next non-separator in case this path has redundant separators. */
  while ((result >= 0) && kiokuCHAR_ISDIRSEP(path[result]))
  {
    result--;
  }
  if ((result >= 0) && !kiokuCHAR_ISDIRSEP(path[result]))
  {
    /* March forward by one so the string ends with a separator */
    result++;
  }
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
