#include "kioku/types.h"
#include "kioku/filesystem.h"
#include "kioku/datastructure.h"
#include "kioku/log.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef kiokuOS_WINDOWS
#include <direct.h>
#include <io.h>
/* Microsoft (correctly) refuses to recognize a number of functions as being ISO C compliant,
   despite being part of POSIX, and recommends its own standard-compliant name. */
#define strdup _strdup
#define rmdir _rmdir
#define getcwd _getcwd
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

static srsMEMSTACK dirstack = {0};
static char *directory_current = NULL;

static void ClearDirectoryStack()
{
  if (dirstack.memory == NULL)
  {
    bool ok = srsMemStack_Init(&dirstack, sizeof(char *), -1);
    srsASSERT(ok);
  }
  size_t i = 0;
  char *string_ptr = NULL;
  while (dirstack.count > 0)
  {
    srsMemStack_Pop(&dirstack, &string_ptr);
    if (string_ptr != NULL)
    {
      free(string_ptr);
    }
  }
}

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

char *srsDir_GetSystemCWD(char *buf, size_t bufsize)
{
  if (buf == NULL)
  {
    return NULL;
  }
  if (bufsize == 0)
  {
    return NULL;
  }
  char *cwd = getcwd(buf, bufsize);
  if (cwd != buf || cwd == NULL)
  {
    return NULL;
  }
  kioku_path_replace_separators(cwd, bufsize);
  return cwd;
}

bool srsDir_SetSystemCWD(const char *buf)
{
  /* This is low level API function, so we assume all input is perfect, more or less */
  srsASSERT(buf != NULL);
  errno = 0;
  bool ok = (chdir(buf) == 0);
  uint32_t errno_capture = errno;
  if (errno_capture != 0)
  {
    srsLOG_ERROR("Failed to change CWD to [%s] (errno = %d): %s", buf, errno_capture, strerror(errno_capture));
  }
  return ok;
}

const char *srsDir_GetCWD()
{
  if (directory_current == NULL)
  {
    directory_current = malloc(kiokuPATH_MAX);
    if (directory_current != NULL)
    {
      char *cwd = srsDir_GetSystemCWD(directory_current, kiokuPATH_MAX);
      if (cwd != directory_current || cwd == NULL)
      {
        srsLOG_ERROR("Failed to get the underlying system CWD when attempting to initialize the API CWD");
        free(directory_current);
        directory_current = NULL;
      }
      else
      {
        srsLOG_NOTIFY("Initialized Kioku CWD to %s", directory_current);
      }
    }
    else
    {
      srsLOG_ERROR("Failed to allocate space for CWD");
    }
  }
  /** @todo Consider doing a realloc to save a little heap space */
  return directory_current;
}

const char *srsDir_SetCWD(const char *path)
{
  /* Clear stack */
  ClearDirectoryStack();
  /* Free the current directory so our next call to srsDir_GetCWD regenerates it */
  free(directory_current);
  directory_current = NULL;
  /* Attempt to change the directory, and if it succeeds cause the current directory to be reallocated */
  if ((path != NULL) && srsDir_SetSystemCWD(path))
  {
    srsLOG_NOTIFY("Set Kioku CWD to %s", directory_current);
    return srsDir_GetCWD();
  }
  /* Failure to do the actual directory changing returns NULL */
  return NULL;
}

const char *srsDir_PushCWD(const char *path)
{
  const char *cwd = NULL;
  /* Invalid path cannot be changed to */
  if (!srsDir_Exists(path))
  {
    srsLOG_ERROR("Failed attempt to navigate to invalid directory: %s", path);
    return NULL;
  }
  /* Initialize the dirstack if it isn't */
  if (dirstack.memory == NULL)
  {
    srsLOG_NOTIFY("Directory Stack appears to be uninitialized - run srsMemStack_Init");
    bool ok = srsMemStack_Init(&dirstack, sizeof(char *), -1);
    srsASSERT(ok);
    srsASSERT(dirstack.element_size > 0);
    srsASSERT(dirstack.capacity > 0);
    srsASSERT(dirstack.memory != NULL);
    srsMEMSTACK_PRINT(dirstack);
  }
  /* Get the current working directory to be pushed (so that a subsequent pop can restore it) */
  cwd = srsDir_GetCWD();
  srsMEMSTACK_PRINT(dirstack);
  srsASSERT(cwd != NULL);
  /** TODO Check result of srsDir_GetCWD */
  char *push_me = strdup(cwd);
  /** TODO Check result of strdup - not exactly sure how best to handle it. */
  srsASSERT(push_me != NULL);
  srsASSERT(dirstack.memory != NULL);
  srsMEMSTACK_PRINT(dirstack);
  /* Try to push the current directory onto the stack before navigating to the new one*/
  if (!srsMemStack_Push(&dirstack, &push_me))
  {
    srsLOG_ERROR("Failed attempt to push directory onto stack: %s", push_me);
    srsMEMSTACK_PRINT(dirstack);
    free(push_me);
    return NULL;
  }
  /* Try changing to the directory we're pushing */
  if ((path != NULL) && srsDir_SetSystemCWD(path))
  {
    /* Free the current directory so our next call to srsDir_GetCWD regenerates it */
    free(directory_current);
    directory_current = NULL;
    cwd = srsDir_GetCWD();
    srsASSERT(cwd != NULL);
    srsLOG_NOTIFY("Pushed Directory (%s): CWD = %s", path, cwd);
  }
  else
  {
    srsLOG_NOTIFY("Failed to change to directory %s - undo change to stack!", path);
    bool ok = srsMemStack_Pop(&dirstack, NULL);
    /* TODO There may be a better way to handle this */
    srsASSERT(ok);
  }
  return cwd;
}

/**
 * Pop the Current Working Directory (CWD) from the Directory Stack, changing to the new top CWD if available.
 * If the previous new top is no longer valid, this function will repeat until either a valid one is found, or no CWDs are left on the stack.
 * @param[out] popped A place to store the null-terminated string of the popped CWD. If non-NULL, it will not be freed and it is up to the user to do so. Otherwise it will be automatically freed.
 * @return Whether the directory was popped to a valid directory.
 */
bool srsDir_PopCWD(char **popped)
{
  char *change_to = NULL;
  bool popped_to_valid_dir = false;
  if (dirstack.memory == NULL)
  {
    srsLOG_NOTIFY("Initializing memory stack before popping, perhaps against our better judgement...");
    bool ok = srsMemStack_Init(&dirstack, sizeof(char *), -1);
    srsASSERT(ok);
  }
  /* Perform the following until we pop to a valid directory */
  while (!popped_to_valid_dir)
  {
    srsLOG_NOTIFY("Iterate dirstack for pop...");
    /* If the new top of the stack is NULL, it means we ran out of directories to try changing to, so we break from the loop */
    if (dirstack.top == NULL)
    {
      break;
    }
    srsLOG_NOTIFY("Stack before possibly segfaulty part...");
    srsMEMSTACK_PRINT(dirstack);
    /* Grab the directory to change to */
    change_to = *((char **)dirstack.top);
    srsLOG_NOTIFY("Stack still has a top: %s", change_to);
    /* Attempt to change to the new directory */
    srsLOG_NOTIFY("Try changing to %s as a result of pop", change_to);
    popped_to_valid_dir = srsDir_SetSystemCWD(change_to);
    srsLOG_NOTIFY("srsDir_SetSystemCWD returned %s", popped_to_valid_dir ? "true" : "false");
    if (!popped_to_valid_dir)
    {
      /* Free it and try the next one. */
      srsLOG_ERROR("Unable to pop directory to %s - try the next one", change_to);
      free(change_to);
      change_to = NULL;
    }
    else
    {
      srsLOG_NOTIFY("Popped Directory: CWD = %s", change_to);
      /* Reset CWD so next call to srsDir_GetCWD regenerates it */
      free(directory_current);
      directory_current = NULL;
    }
    /* If we can't pop, there's nothing left to try changing to and must break out of the loop. */
    if (!srsMemStack_Pop(&dirstack, NULL))
    {
      break;
    }
    srsLOG_NOTIFY("Pop worked!");
  }
  if (popped != NULL)
  {
    srsLOG_NOTIFY("Storing result of srsDir_PopCWD: %s (DON'T FORGET TO FREE)", change_to);
    *popped = change_to;
  }
  return popped_to_valid_dir;
}

/** @todo Perhaps have a method called by an init that dynamically finds a "true" max path length */

FILE *srsFile_Open(const char *path, const char *mode)
{
#ifdef kiokuOS_WINDOWS
  FILE *fp = NULL;
  fopen_s(&fp, path, mode);
#else
  FILE *fp = fopen(path, mode);
#endif
  return fp;
}

/** @todo It may be a good idea to have a max path length and use a strnlen-like method. */
/** @todo implement a relative path resolver that eliminates . and .. - https://linux.die.net/man/3/realpath */

/**
 * Find the full path from a relative one, limiting the storage of it to nbytes.
 * @param[in] relative The relative path to transform.
 * @param[out] path_out The place to store the full string.
 * @param[out] nbytes The maximum number of characters to store, including the null terminator.
 * @return The number of characters that were needed for the full path, excluding the null terminator. Setting path_out to NULL or nbytes to 0 will still cause this to output how many characters were needed.
 */
size_t srsPath_GetFull(const char *relative, char *path_out, size_t nbytes)
{
  size_t result = 0;
  /** @todo mitigate max path length using the getcwd and chdir method described below for all platforms */
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
  /* Consider using realpath and _fullpath */
  if (relative == NULL)
  {
    return result;
  }
  if (path_out == NULL)
  {
    return result;
  }
  if (nbytes == NULL)
  {
    return result;
  }
#ifdef kiokuOS_WINDOWS
  /** @todo Test this */
  if (relative[0] == '.')
  {
    relative++;
    while (relative[0] == '/')
    {
      relative++;
    }
  }
  char *fullpath = _fullpath(path_out, relative, nbytes);
  if (fullpath != NULL)
  {
    int32_t needed = strlen(fullpath);
    if (needed > 0)
    {
      result = (size_t) needed;
    }
    else
    {
      srsLOG_ERROR("The calculated needed size to store the full path of %s was <= 0 - this is likely a defect - Please report this!", relative);
      srsBAIL();
    }
  }
#else
  #if !defined(kiokuOS_APPLE) && _POSIX_VERSION < 200809L
    #error Linux-like implementation requires POSIX 2008+ for a safe realpath implementation!
  #endif
  const char *cwd = srsDir_GetCWD();
  if (cwd != NULL)
  {
    /* TODO This is a serious vulnerability, just as anything that uses bounded path sizes. There are mitigations for this in POSIX 2001 but no real soltion. In POSIX 2008 this can be significantly safer by having the implementation malloc for us. */
    char *path = strdup(cwd);
    kioku_path_replace_separators(path, strlen(path) + 1);
    int32_t needed = kioku_path_concat(path_out, nbytes, path, relative);
    if (needed > 0)
    {
      result = (size_t) needed;
    }
    free(path);
    /* Instead of checking POSIX spec, let compilation fail for missing realpath. */
    int errno_capture = 0;
    errno = 0;
    path = realpath(path_out, NULL);
    errno_capture = errno;
    /* TODO Add a check for whether realpath behaves as described by POSIX 2008 in CMakeLists.txt if such a facility exists so we don't fail preprocessor checks in funky compilers. */
    #if defined(kiokuOS_APPLE) && (_POSIX_VERSION < 200809L)
    if (errno_capture != 0)
    {
      /* QUOTH THE SPEC
           EINVAL
           path  is NULL.   (In  glibc versions  before 2.3,  this
                             error is also returned if resolved_path is NULL.)
      */
      /* Here the input for path (path_out) cannot be NULL at this point, so the realpath implementation doesn't support returning a malloc'd string */
      srsLOG_ERROR("Something went wrong calling realpath: %d = %s", errno_capture, strerror(errno_capture));
      if (errno_capture == EINVAL)
      {
        /* So here we assert that path != NULL, but here it will almost certainly be. Failure here indicates an unsupported realpath implementation. */
        srsLOG_ERROR("Unsupported realpath implementation!");
        /* TODO This is sloppy, but best solution I can think of for now. Needs to be more robust some day. */
        srsASSERT(path != NULL);
      }
    }
    #endif
    needed = 0;
    if (path != NULL)
    {
      needed = strlen(path);
      if (needed > 0)
      {
        result = (size_t) needed;
      }
      else
      {
        srsLOG_ERROR("The calculated needed size to store the full path of %s was <= 0 - this is likely a defect - Please report this!", relative);
        srsBAIL();
      }
      strcpy(path_out, path);
      free(path);
    }
    /** @todo resolve the relative paths to mimic the functionality of realpath and _fullpath */
    /* result = kioku_path_resolve_relative(path_out, result); */
  }
#endif
  /** @todo resolve the relative paths to mimic the functionality of realpath and _fullpath if necessary. */
  if (result > 0)
  {
    kioku_path_replace_separators(path_out, nbytes);
  }
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
    while ((path_end > path_start) && srsCHAR_ISDIRSEP(path[path_end]))
    {
      path_end--;
    }
    /* Strip extra slashes at start of path */
    while ((path_start < path_end) && srsCHAR_ISDIRSEP(path[path_start]))
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
  if (srsCHAR_ISDIRSEP(path2[path2_start]))
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
  if ((neededlen > 0) && !srsCHAR_ISDIRSEP(path1[path1_end]) && (path2[path2_start] != '\0'))
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
  /** @todo Perhaps allow user to specify a string length, though start_index is probably sufficient for that purpose. */

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
  /** @todo Test this */
  if (start_index < 0 || start_index >= INT32_MAX)
  {
    result = INT32_MIN;
    return result;
  }
  /* First eliminate dangling NULLs (if any) */
  /** @todo Test this */
  result = start_index;
  while ((result >= 0) && (path[result] == kiokuCHAR_NULL))
  {
    result--;
  }
  /* Sanity check: If it hits the beginning of the string, it was apparently empty - this case should be captured above. */
  assert(result >= 0);
  /* If the path ends with a separator, we need to march back to the first non-separator before traversing to the parent. */
  if (srsCHAR_ISDIRSEP(path[result]))
  {
    /* March back to first non-separator */
    while ((result >= 0) && srsCHAR_ISDIRSEP(path[result]))
    {
      result--;
    }
  }
  /* March back to first separator to eliminate the file/dir we're on in the path. */
  while ((result >= 0) && !srsCHAR_ISDIRSEP(path[result]))
  {
    result--;
  }
  /* March back to next non-separator in case this path has redundant separators. */
  while ((result >= 0) && srsCHAR_ISDIRSEP(path[result]))
  {
    result--;
  }
  if ((result >= 0) && !srsCHAR_ISDIRSEP(path[result]))
  {
    /* March forward by one so the string ends with a separator */
    result++;
  }
  return result;
}

/**
 * Get the path to the directory where a Kioku user's collection is stored.
 * @param[in] username The keyname of the user to get the path for.
 * @return The stored path to their directory, which may or may not exist.
 */
bool srsDir_Create(const char *path)
{
  bool ok = false;
  char *create_dir = NULL;
  if (path == NULL)
  {
    srsLOG_ERROR("Input path was NULL");
    goto done;
  }
  create_dir = strdup(path);
  if (create_dir == NULL)
  {
    srsLOG_ERROR("Failed to duplicate string!");
    goto done;
  }
  char *next_separator = create_dir;
  while ((next_separator != NULL) && (*next_separator != '\0'))
  {
    while (!srsCHAR_ISDIRSEP(*next_separator) &&  /* Stop at the first separator */
           (*next_separator != '\0') )  /* Or stop at the first null terminator */
    {
      next_separator++;
    }
    /** @todo EDGE CASE: two separators in a row (this includes / followed by \0) */
    char separator_character = *next_separator;
    *next_separator = '\0';
    /* @todo Check if create_dir at this point is a file and fail if it is */
    if (srsFile_Exists(create_dir))
    {
      ok = false;
      srsLOG_ERROR("Attempted to create dir `%s`, but a file with that name already exists!", create_dir);
      goto done;
    }
    srsLOG_NOTIFY("Attempt to create dir `%s`", create_dir);
#ifdef kiokuOS_WINDOWS
    ok = _mkdir(create_dir) == 0;
#else
    ok = mkdir(create_dir, 0700) == 0;
#endif
    if (!ok)
    {
      srsLOG_ERROR("Error occurred while attempting to create directory [%s]: %s", create_dir, strerror(errno));
    }
    /* We don't do anything with ok for now. If we break the loop here we won't get to attempt all the directories we need to. */
    *next_separator = separator_character;
    if (separator_character != '\0')
    {
      next_separator++;
    }
  }
done:
  if (ok)
  {
    ok = srsDir_Exists(create_dir);
    if (!ok)
    {
      srsLOG_ERROR("For some reason, the system function for directory creation succeeded, yet our check for directory existence failed. This could be a serious defect. Please report this!");
      srsBAIL();
    }
  }
  if (create_dir != NULL)
  {
    if (!ok)
    {
      srsLOG_NOTIFY("Failed to create final dir `%s`", create_dir);
    }
    free(create_dir);
  }
  return ok;
}

/**
 * Create an empty directory along with leading parent directories if necessary.
 * @param[in] path Path to the dir to create.
 * @return Whether the path and preceding directories could be created.
 */
bool srsFile_Create(const char *path)
{
  /* Cannot create a NULL path */
  if (path == NULL)
  {
    return false;
  }
  /* Cannot create something that already exists */
  if (srsPath_Exists(path))
  {
    return false;
  }
  /* This function is not for creating an empty directory */
  size_t pathlen = strlen(path);
  if (pathlen == 0)
  {
    return false;
  }
  /* Cannot create a directory */
  if (srsCHAR_ISDIRSEP(path[pathlen-1]))
  {
    return false;
  }
  /* Calculate path to the parent dir of the new file */
  /** @todo Consider moving this functionality to an srsPath_GetParentDir method */
  char *create_dir = strdup(path);
  size_t path_length = strlen(create_dir);
  char *end_of_dirs = create_dir + path_length;
  while ((end_of_dirs > create_dir) && /* Do not iterate beyond the start of the path */
         (!srsCHAR_ISDIRSEP(*end_of_dirs))) /* Stop at the first separator */
  {
    end_of_dirs--;
  }
  *end_of_dirs = '\0';
  /* Create the parent dir */
  bool ok = srsDir_Create(create_dir);
  free(create_dir);
  /* Create the file */
  FILE *fp = srsFile_Open(path, "w");
  /* Return whether successful */
  return (fp != NULL) && (fclose(fp) == 0);
}

bool srsPath_Move(const char *path, const char *newpath)
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
bool srsPath_Remove(const char *path)
{
  bool result = false;
  if (path == NULL)
  {
    return result;
  }
  if (!srsPath_Exists(path))
  {
    return result;
  }
  if (srsDir_Exists(path))
  {
    result = rmdir(path) == 0;
  }
  else
  {
    result = remove(path) == 0;
  }
  return result;
}

bool srsPath_Exists(const char *path)
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

/** @todo Add tests */
bool srsFile_Exists(const char *path)
{
  bool result = false;
  if (path == NULL)
  {
    return result;
  }
  if (!srsPath_Exists(path))
  {
    return result;
  }
  result = !srsDir_Exists(path);
  return result;
}

bool srsDir_Exists(const char *path)
{
  bool result = false;
  if (path == NULL)
  {
    return result;
  }
  if (!srsPath_Exists(path))
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

int32_t srsFile_GetLength(const char *filepath)
{
  int32_t result = -1;
  if (filepath == NULL)
  {
    return result;
  }
  if (!srsPath_Exists(filepath))
  {
    return result;
  }
  if (srsDir_Exists(filepath))
  {
    return result;
  }
  FILE *fp = srsFile_Open(filepath, "r");
  if (fp != NULL)
  {
    fseek(fp, 0, SEEK_END);
    result = ftell(fp);
  }
  fclose(fp);
  return result;
}

bool srsFile_SetContent(const char *filepath, const char *content)
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
  if (!srsPath_Exists(filepath))
  {
    return result;
  }
  if (srsDir_Exists(filepath))
  {
    return result;
  }
  FILE *fp = srsFile_Open(filepath, "w");
  if (fp == NULL)
  {
    return result;
  }
  result = fputs(content, fp) >= 0;
  fclose(fp);
  return result;
}

bool srsFile_GetContent(const char *filepath, char *content_out, size_t count)
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
  if (!srsPath_Exists(filepath))
  {
    return result;
  }
  if (srsDir_Exists(filepath))
  {
    return result;
  }
  FILE *fp = srsFile_Open(filepath, "r");
  if (fp == NULL)
  {
    return result;
  }
  char *str = fgets(content_out, count, fp);
  result = str == content_out;
  fclose(fp);
  return result;
}

int32_t srsFile_ReadLineByNumber(const char *path, uint32_t linenum, char *linebuf, size_t linebuf_size)
{
  /* Initialize results */
  int32_t result = -1;
  size_t linelen = 0;
  FILE *fp = NULL;
  /* Must be valid line number */
  if (linenum < 1)
  {
    goto done;
  }
  /* Must be valid buffer */
  if (linebuf == NULL)
  {
    goto done;
  }
  /* Must be valid buffer size */
  if (linebuf_size == 0)
  {
    goto done;
  }
  /* Attempt to open file */
  fp = srsFile_Open(path, "r");
  /* Must be valid file */
  if (fp == NULL)
  {
    srsLOG_ERROR("Failed to open %s for line reading", path);
    goto done;
  }
  int ch = EOF;
  int i = 1;
  /* Seek to the line */
  for (ch = fgetc(fp); (i < linenum) && (ch != EOF); ch = fgetc(fp))
  {
    if (ch == '\n')
    {
      i++;
    }
  }
  /* If we reached the end of file, it means the line number was greater than the number of lines in the file */
  if (ch == EOF)
  {
    goto done;
  }
  /* Read the line into the buffer up to buffer size excluding null terminator, and stripping carriage returns. */
  for (; (linelen < linebuf_size-1) && (ch != '\n') && (ch != EOF); ch = fgetc(fp))
  {
    if (ch != '\r')
    {
      linebuf[linelen] = (char) ch;
      linelen++;
    }
  }
  /* No freaking way will this be greater than maximum int32... unless it is. That would be bad. */
  assert(linelen <= INT32_MAX);
  result = (int32_t) linelen;
  /* Perform cleanup */
done:
  /* Don't null-terminate an ostensibly zero-sized buffer. */
  if ((linebuf != NULL) && (linebuf_size > 0))
  {
    linebuf[linelen] = '\0';
  }
  /* Don't close an invalid file handle */
  if (fp != NULL)
  {
    fclose(fp);
  }
  return result;
}

#include "tinydir.h"
static bool srsFileSystem_Iterate_Internal(const char *dirpath, void *userdata, srsFILESYSTEM_VISIT_FUNC iterate, bool *exit_out, size_t *depth_out)
{
  bool result = false;
  tinydir_dir dir;
  const int tinydir_invalid_result = -9999;
  int tinydir_result = tinydir_invalid_result;
  if (dirpath == NULL)
  {
    goto done;
  }
  if (iterate == NULL)
  {
    goto done;
  }
  if (!srsDir_Exists(dirpath))
  {
    goto done;
  }
  tinydir_result = tinydir_open(&dir, dirpath);
  if (tinydir_result == -1)
  {
    goto done;
  }
  char *cwd = srsDir_PushCWD(dirpath);
  if (cwd == NULL)
  {
    goto done;
  }
  *depth_out += 1;
  srsLOG_NOTIFY("Iterating in %s (%s)", dirpath, cwd);
  result = true;
  while (dir.has_next)
  {
    tinydir_file file;
    errno = 0;
    tinydir_result = tinydir_readfile(&dir, &file);
    int errno_capture = errno;
    if (tinydir_result == -1)
    {
      result = false;
      srsLOG_ERROR("tinydir_readfile had error getting file: %s", strerror(errno_capture));
      goto done;
    }
    /* Do not risk endless loop recursing on the current or parent directory */
    if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
    {
      tinydir_next(&dir);
      continue;
    }
    srsLOG_NOTIFY("Visiting %s", file.name);
    /* Perform one iteration to see what the user wants to do from here */
    srsFILESYSTEM_VISIT_ACTION action = iterate(file.name, userdata);
    switch(action)
    {
      /* The user wants to recurse into a directory if possible */
      case srsFILESYSTEM_VISIT_RECURSE:
      {
        if (file.is_dir)
        {
          srsLOG_NOTIFY("RECURSE -> %s", file.name);
          /* Traverse into directory */
          result = srsFileSystem_Iterate_Internal(file.name, userdata, iterate, exit_out, depth_out);
          if (!result || *exit_out)
          {
            srsLOG_NOTIFY("Breaking out of iteration (result = %s, exit = %s)", result ? "true" : "false", *exit_out ? "true" : "false");
            goto done;
          }
          break;
        }
        /* Fall through to continue */
      }
      /* The user wants to continue onto the next item */
      case srsFILESYSTEM_VISIT_CONTINUE:
      {
        srsLOG_NOTIFY("CONTINUE processing %s (%s)", dirpath, cwd);
        break;
      }
      case srsFILESYSTEM_VISIT_STOP:
      {
        /* The user wants to break out of directory being traversed, which may mean picking up from another one. */
        srsLOG_NOTIFY("STOP processing %s (%s)", dirpath, cwd);
        goto done;
      }
      case srsFILESYSTEM_VISIT_EXIT:
      {
        *exit_out = true;
        srsLOG_NOTIFY("EXIT all processing from %s (%s) upward", dirpath, cwd);
        goto done;
      }
    }
    tinydir_next(&dir);
  }
done:
  if (srsDir_PopCWD(NULL))
  {
    *depth_out -= 1;
  }
  if (tinydir_result != tinydir_invalid_result)
  {
    tinydir_close(&dir);
  }
  return result;
}

bool srsFileSystem_Iterate(const char *dirpath, void *userdata, srsFILESYSTEM_VISIT_FUNC iterate)
{
  bool result = false;
  bool exit_triggered = false;
  size_t depth = 0;
  result = srsFileSystem_Iterate_Internal(dirpath, userdata, iterate, &exit_triggered, &depth);
  while (depth > 0)
  {
    srsLOG_NOTIFY("Must pop %zu more directories to store CWD to what it was before srsFileSystem_Iterate was called", depth);
    /** TODO There's a serious problem with the @ref srsDir_PopCWD behaviour for inaccessible files being opaque. Not only could it screw up depth_out, but also cause the iterate function to behave unpredictably. */
    srsDir_PopCWD(NULL);
    depth--;
  }
  return result;
}
