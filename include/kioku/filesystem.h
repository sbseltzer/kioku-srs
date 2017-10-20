#ifndef _KIOKU_FILESYSTEM_H
#define _KIOKU_FILESYSTEM_H

#include "kioku/decl.h"
#include "kioku/types.h"
#include "kioku/log.h"
#include "kioku/debug.h"

#include <stdio.h> /* FILE, fopen, fclose, fgets */

#if defined kiokuOS_WINDOWS /* OS check */
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #if defined _MAX_PATH
    #define kiokuPATH_MAX (_MAX_PATH - 1)
  #elif defined MAX_PATH
    #define kiokuPATH_MAX (MAX_PATH - 1)
  #else
    #error Could not find macro for max path length!
  #endif
#elif defined kiokuOS_APPLE  /* OS check */
  /* https://stackoverflow.com/questions/7140575/mac-os-x-lion-what-is-the-max-path-length#16905266 */
  #include <sys/syslimits.h>
#elif defined kiokuOS_LINUX || defined kiokuOS_UNIX /* OS check */
  #include <dirent.h>
  /* Linux defines PATH_MAX here */
  #include <linux/limits.h>
  /* Though POSIX 2008 defines PATH_MAX here */
  /** \todo Add an ifdef for posix vs linux */
  #include <limits.h>
  /* And yet some sources say it's in here */
  /* #include <sys/syslimits.h> */
#endif /* OS check */

#ifndef kiokuPATH_MAX
  #ifdef PATH_MAX
    #define kiokuPATH_MAX PATH_MAX - 1
  #else
    #error Could not find macro for max path length!
  #endif
#endif

#define kiokuCHAR_ISDIRSEP(x) (((x) == '/') || ((x) == '\\'))

kiokuSTATIC_ASSERT(kiokuPATH_MAX > 0);

/** Find the full path from a relative one, limiting the storage of it to nbytes.
 * \param[in] relative The relative path to transform.
 * \param[out] path_out The place to store the full string.
 * \param[out] nbytes The maximum number of characters to store, including the null terminator.
 * \return The number of characters that were needed for the full path, excluding the null terminator. Setting path_out to NULL or nbytes to 0 will still cause this to output how many characters were needed.
 */
kiokuAPI size_t kioku_path_getfull(const char *relative, char *path_out, size_t nbytes);

/** Find the first and last chars to use in path for trimming redundant chars on each end.
 * Up to one leading slash may remain.
 * No trailing slashes past a first leading slash may remain.
 * \param[in] path Path to find trim points on.
 * \param[out] start Place to store the new start of the path string.
 * \param[out] end Place to store the new end of the path string.
 */
kiokuAPI void kioku_path_trimpoints(const char *path, size_t *start, size_t *end);

/** Concatenate two paths, storing the result in dest.
 * \param[out] dest Place to store the resulting path.
 * \param[in] destsize Number of bytes available for storage in dest.
 * \param[in] path1 Left side of the path to store.
 * \param[in] path2 Right side of the path to store.
 * \return Number of bytes placed in dest, excluding the NULL byte ('\0') at the end. -1 is returned if any of the pointers are NULL. If there isn't enough room to store the concatenated path, it will return how much space was needed. In such cases the contents of dest are unspecified.
 */
kiokuAPI int32_t kioku_path_concat(char *dest, size_t destsize, const char *path1, const char *path2);

/** Get the index of the final directory separator in the specified path.
 * This is used to assist in traversing up a directory tree.
 * Because returning 0, -1, and less that -1 mean different things, the proper way to utilize this iteratively is as follows:
 *   for (i = strlen(path); i > -1; i = kioku_path_up_index(path, i)) { do stuff }
 *   if (i < -1) { you likely passed in an improper path - handle case }
 * \param[in] path The path string to traverse. Must be null terminated.
 * \param[in] start_index The index to start traversing from. -1 means start from the end.
 * \return Index of last path separator. -1 if path is NULL, path is empty, or if there are no separators left. If path length exceeds the size of a 32-bit integer (which is a remote possibility in the case of a non-null-terminated string, though frankly you should be checking your input), a value less than -1 will be returned.
 */
kiokuAPI int32_t kioku_path_up_index(const char *path, int32_t start_index);

/** The concept of a user home directory isn't really a cross-platform concept.
 * To make this portable, we will define a method that determines a Kioku home directory.
 * Kioku needs a safe place where it has permission to read/write and store plugins/configurations.
 * The user can change where users are stored, but the file that lists those locations needs a home.
 * Useful Info:
 * - http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
 * - https://stackoverflow.com/a/3733955
 * - https://stackoverflow.com/a/933996
 * - https://github.com/ehsan/ogre/tree/master/Samples/Browser/src
 * - https://github.com/seltzy/orx/blob/master/code/src/io/orxFile.c
 * \return Path to a consistent directory where Kioku can safely do its work undisturbed.
 */
kiokuAPI const char *kioku_get_home_directory();

/** Get the path to the directory where a Kioku user's collection is stored.
 * \param[in] username The keyname of the user to get the path for.
 * \return The stored path to their directory, which may or may not exist.
 */
kiokuAPI const char *kioku_get_user_directory(const char *username);

/** Create an empty file along with leading parent directories if necessary.
 * Because git doesn't see directories until there are files in them, why should the filesystem module?
 * \param[in] path Path to the file to create.
 * \return Whether the path and preceding directories could be created.
 */
kiokuAPI bool kioku_filesystem_create(const char *path);

/** Rename a file or directory
 * \param[in] path Path to the file/dir to rename.
 * \return Whether the file/dir was renamed.
 */
kiokuAPI bool kioku_filesystem_rename(const char *path, const char *newpath);

/** Delete a file or directory. Directories will be deleted recursively.
 * \param[in] path Path to the file/dir to delete.
 * \return Whether the file/dir was deleted.
 */
kiokuAPI bool kioku_filesystem_delete(const char *path);

/** Check if a file/dir exists.
 * \param[in] path Path to the file/dir to check.
 * \return Whether the file/dir exists.
 */
kiokuAPI bool kioku_filesystem_exists(const char *path);

/** Check if path is a dir.
 * \param[in] path Path to the file/dir to check.
 * \return Whether the file/dir is a directory. It will also return false if it doesn't exist.
 */
kiokuAPI bool kioku_filesystem_isdir(const char *path);

/** Gets the length of a file
 * \param[in] filepath Path to the file
 * \return If the file can be opened and the seek/tell is successful, it will return the length. -1 otherwise.
 */
kiokuAPI int32_t kioku_filesystem_getlen(const char *filepath);

/** Set the content of a file (overwriting what may have been there)
 * \param[in] filepath Path to the file
 * \param[in] content The null-terminated content string to write.
 * \return Whether successful
 */
kiokuAPI bool kioku_filesystem_setcontent(const char *filepath, const char *content);

/** Get the content of a file.
 * \param[in] filepath Path to the file
 * \param[out] content Where to read the string into. Must be able to fit the value of readlen plus a null terminator.
 * \param[in] count Size of the content buffer, including null-terminator.
 * \return Whether successful
 */
kiokuAPI bool kioku_filesystem_getcontent(const char *filepath, char *content_out, size_t count);

kiokuAPI FILE *kioku_filesystem_open(const char *path, const char *mode);

#ifndef srsFILESYSTEM_DIRSTACK_MAX
#define srsFILESYSTEM_DIRSTACK_MAX 16
#endif

/** Get the Current Working Directory (CWD)
 * \return Null-terminated string. Do not attempt to free it, as it is memory-managed. NULL is returned if the last known CWD has become invalid.
 */
kiokuAPI const char *srsDir_GetCurrent();

/** Set the Current Working Directory (CWD)
 * This will clear the Directory Stack.
 * \param[in] path Directory path (can be relative) to change to. It will not be stored.
 * \return The null-terminated string of the new CWD. Do not attempt to free it, as it is memory-managed. NULL is returned if the path is invalid.
 */
kiokuAPI const char *srsDir_SetCurrent(const char *path);

/** Push a new Current Working Directory (CWD) onto the Directory Stack, changing to it.
 * If the stack exceeds its limit (as defined by srsFILESYSTEM_DIRSTACK_MAX), it will be lost.
 * \param[in] path Directory path (can be relative) to change to. It will not be stored.
 * \param[in] lost A place to store the path that was lost in the case the user exceeds the max directory stack size. If this is NULL, the bottom of the stack can be silently lost.
 * \return The null-terminated string of the new CWD. Do not attempt to free it, as it is memory-managed. NULL is returned if the path is invalid.
 */
kiokuAPI const char *srsDir_PushCurrent(const char *path, const char **lost);

/** Pop the Current Working Directory (CWD) from the Directory Stack, changing to the new top CWD if available.
 * If the previous new top is no longer valid, this function will repeat until either a valid one is found, or no CWDs are left on the stack.
 * \return The null-terminated string of the previous CWD. Do not attempt to free it, as it is memory-managed. NULL is returned if the stack becomes empty.
 */
kiokuAPI const char *srsDir_PopCurrent();

#endif /* _KIOKU_FILESYSTEM_H */
