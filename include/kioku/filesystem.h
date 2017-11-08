/**
 * @addtogroup Filesystem
 *
 * Filesystem module
 * Module that handles file system access
 *
 * @{
 */

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
  /* Linux supposedly defines PATH_MAX here */
  /* #include <linux/limits.h> */
  /* Though POSIX 2008 defines PATH_MAX here */
  /** @todo Add an ifdef for posix vs linux */
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

#define srsCHAR_ISDIRSEP(x) (((x) == '/') || ((x) == '\\'))

kiokuSTATIC_ASSERT(kiokuPATH_MAX > 0);

/**
 * Initialize filesystem module state.
 * @return Whether all went ok.
 */
kiokuAPI bool srsFileSystem_Init();

/**
 * Cleanup filesystem module state.
 * @return Whether all went ok.
 */
kiokuAPI bool srsFileSystem_Exit();

/**
 * Find the full path from a relative one, limiting the storage of it to nbytes.
 * @param[in] relative The relative path to transform.
 * @param[out] path_out The place to store the full string.
 * @param[out] nbytes The maximum number of characters to store, including the null terminator.
 * @return The number of characters that were needed for the full path, excluding the null terminator. Setting path_out to NULL or nbytes to 0 will still cause this to output how many characters were needed.
 */
kiokuAPI size_t srsPath_GetFull(const char *relative, char *path_out, size_t nbytes);

/**
 * Find the first and last chars to use in path for trimming redundant chars on each end.
 * Up to one leading slash may remain.
 * No trailing slashes past a first leading slash may remain.
 * @param[in] path Path to find trim points on.
 * @param[out] start Place to store the new start of the path string.
 * @param[out] end Place to store the new end of the path string.
 */
kiokuAPI void kioku_path_trimpoints(const char *path, size_t *start, size_t *end);

/**
 * Concatenate two paths, storing the result in dest.
 * @param[out] dest Place to store the resulting path.
 * @param[in] destsize Number of bytes available for storage in dest.
 * @param[in] path1 Left side of the path to store.
 * @param[in] path2 Right side of the path to store.
 * @return Number of bytes placed in dest, excluding the NULL byte ('\0') at the end. -1 is returned if any of the pointers are NULL. If there isn't enough room to store the concatenated path, it will return how much space was needed. In such cases the contents of dest are unspecified.
 */
kiokuAPI int32_t kioku_path_concat(char *dest, size_t destsize, const char *path1, const char *path2);

/**
 * Get the index of the final directory separator in the specified path.
 * This is used to assist in traversing up a directory tree.
 * Because returning 0, -1, and less that -1 mean different things, the proper way to utilize this iteratively is as follows:
 *   for (i = strlen(path); i > -1; i = kioku_path_up_index(path, i)) { do stuff }
 *   if (i < -1) { you likely passed in an improper path - handle case }
 * @param[in] path The path string to traverse. Must be null terminated.
 * @param[in] start_index The index to start traversing from. -1 means start from the end.
 * @return Index of last path separator. -1 if path is NULL, path is empty, or if there are no separators left. If path length exceeds the size of a 32-bit integer (which is a remote possibility in the case of a non-null-terminated string, though frankly you should be checking your input), a value less than -1 will be returned.
 */
kiokuAPI int32_t kioku_path_up_index(const char *path, int32_t start_index);

/**
 * The concept of a user home directory isn't really a cross-platform concept.
 * To make this portable, we will define a method that determines a Kioku home directory.
 * Kioku needs a safe place where it has permission to read/write and store plugins/configurations.
 * The user can change where users are stored, but the file that lists those locations needs a home.
 * Useful Info:
 * - http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
 * - https://stackoverflow.com/a/3733955
 * - https://stackoverflow.com/a/933996
 * - https://github.com/ehsan/ogre/tree/master/Samples/Browser/src
 * - https://github.com/seltzy/orx/blob/master/code/src/io/orxFile.c
 * @return Path to a consistent directory where Kioku can safely do its work undisturbed.
 */
kiokuAPI const char *kioku_get_home_directory();

/**
 * Get the path to the directory where a Kioku user's collection is stored.
 * @param[in] username The keyname of the user to get the path for.
 * @return The stored path to their directory, which may or may not exist.
 */
kiokuAPI const char *kioku_get_user_directory(const char *username);

/**
 * Create an empty directory along with leading parent directories if necessary.
 * @param[in] path Path to the dir to create.
 * @return Whether the path and preceding directories could be created.
 */
kiokuAPI bool srsDir_Create(const char *path);

/**
 * Create an empty file along with leading parent directories if necessary.
 * @param[in] path Path to the file to create.
 * @return Whether the path and preceding directories could be created.
 */
kiokuAPI bool srsFile_Create(const char *path);

/**
 * Rename a file or directory
 * @param[in] path Path to the file/dir to rename.
 * @return Whether the file/dir was renamed.
 */
kiokuAPI bool srsPath_Move(const char *path, const char *newpath);

/**
 * Delete a file or directory. @todo Directories will be deleted recursively.
 * @param[in] path Path to the file/dir to delete.
 * @return Whether the file/dir was deleted.
 */
kiokuAPI bool srsPath_Remove(const char *path);

/**
 * Check if a file/dir exists.
 * @param[in] path Path to the file/dir to check.
 * @return Whether the file/dir exists.
 */
kiokuAPI bool srsPath_Exists(const char *path);

/**
 * Check if a file exists.
 * @param[in] path Path to the file/dir to check.
 * @return Whether the file exists.
 */
kiokuAPI bool srsFile_Exists(const char *path);

/**
 * Check if path is an existing dir.
 * @param[in] path Path to the file/dir to check.
 * @return Whether the file/dir is an existing directory.
 */
kiokuAPI bool srsDir_Exists(const char *path);

/**
 * Gets the length of a file
 * @param[in] filepath Path to the file
 * @return If the file can be opened and the seek/tell is successful, it will return the length. -1 otherwise.
 */
kiokuAPI int32_t srsFile_GetLength(const char *filepath);

/**
 * Set the content of a file (overwriting what may have been there)
 * @param[in] filepath Path to the file
 * @param[in] content The null-terminated content string to write.
 * @return Whether successful
 */
kiokuAPI bool srsFile_SetContent(const char *filepath, const char *content);

/**
 * Get the content of a file.
 * @param[in] filepath Path to the file
 * @param[out] content Where to read the string into. Must be able to fit the value of readlen plus a null terminator.
 * @param[in] count Size of the content buffer, including null-terminator.
 * @return Whether successful
 */
kiokuAPI bool srsFile_GetContent(const char *filepath, char *content_out, size_t count);

/**
 * Wrapper for fopen.
 * @param[in] filepath Path to the file.
 * @param[in] mode File descriptor mode. Accepts anything that fopen would.
 * @return Whether successful.
 */
kiokuAPI FILE *srsFile_Open(const char *path, const char *mode);

#ifndef srsFILESYSTEM_DIRSTACK_SIZE
#define srsFILESYSTEM_DIRSTACK_SIZE 16
#endif

/**
 * Get the Current Working Directory (CWD)
 * Essentially a getcwd wrapper.
 * @param[out] buf Buffer to store directory path in.
 * @param[in] bufsize Size of buf including space for null terminator.
 * @return Returns buf if successful - NULL if not.
 */
kiokuAPI char *srsDir_GetSystemCWD(char *buf, size_t bufsize);

/**
 * Set the Current Working Directory (CWD)
 * Essentially a chdir wrapper.
 * @param[in] path Null-terminated path string.
 * @return Returns whether it was successful.
 */
kiokuAPI bool srsDir_SetSystemCWD(const char *path);

/**
 * Get the Current Working Directory (CWD)
 * @return Null-terminated string. Do not attempt to free it, as it is memory-managed. NULL is returned if the last known CWD has become invalid.
 */
kiokuAPI const char *srsDir_GetCWD();

/**
 * Set the Current Working Directory (CWD)
 * This will clear the Directory Stack.
 * @param[in] path Directory path (can be relative) to change to. It will not be stored.
 * @return The null-terminated string of the new CWD. Do not attempt to free it, as it is memory-managed. NULL is returned if the path is invalid.
 */
kiokuAPI const char *srsDir_SetCWD(const char *path);

/**
 * Push a new Current Working Directory (CWD) onto the Directory Stack, changing to it.
 * If the stack exceeds its limit (as defined by srsFILESYSTEM_DIRSTACK_SIZE), it will be lost.
 * @param[in] path Directory path (can be relative) to change to. It will not be stored.
 * @param[out] lost A place to store the path that was lost in the case the user exceeds the max directory stack size. If this is non-NULL and the stored value is non-NULL, it is up to the user to free the value stored. If this is NULL, the bottom of the stack can be silently lost and the memory will be automatically freed.
 * @return The null-terminated string of the new CWD. Do not attempt to free it, as it is memory-managed. NULL is returned if the path is invalid.
 */
kiokuAPI const char *srsDir_PushCWD(const char *path, char **lost);

/**
 * Pop the Current Working Directory (CWD) from the Directory Stack, changing to the new top CWD if available.
 * If the previous new top is no longer valid, this function will repeat until either a valid one is found, or no CWDs are left on the stack.
 * @param[out] popped A place to store the null-terminated string of the popped CWD. If non-NULL, it will not be freed and it is up to the user to do so. Otherwise it will be automatically freed.
 * @return Whether an entry was popped.
 */
kiokuAPI bool srsDir_PopCWD(char **popped);

/**
 * Read a line by it's number from the specified file into a buffer.
 * The content of the buffer will be the specified line (if it exists) with all newlines and carriage returns stripped.
 * @param[in] path The filepath.
 * @param[in] linenum The line number to seek to before reading. Line numbers start at 1. @todo Perhaps have a seek to line number function?
 * @param[out] linebuf The buffer to write the line to.
 * @param[in] linebuf_size The available size of the buffer, including null terminator. If this is zero, the resulting contents of linebuf are unspecified.
 * @return The number of characters written to the buffer, excluding the null terminator. If the file cannot be opened, line does not exist, buffer is NULL, or buffer is said to be zero bytes long, it will return -1.
 */
kiokuAPI int32_t srsFile_ReadLineByNumber(const char *path, uint32_t linenum, char *linebuf, size_t linebuf_size);

#endif /* _KIOKU_FILESYSTEM_H */

/**
 * @} */
