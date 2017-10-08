#ifndef _KIOKU_FILESYSTEM_H
#define _KIOKU_FILESYSTEM_H

#include "kioku/decl.h"
#include "kioku/types.h"
#include "kioku/log.h"

/** Find the first and last chars to use in path for trimming redundant chars on each end.
 * Up to one leading slash may remain.
 * No trailing slashes past a first leading slash may remain.
 * \param[in] path Path to find trim points on.
 * \param[out] start Place to store the new start of the path string.
 * \param[out] end Place to store the new end of the path string.
 */
kiokuAPI void kioku_path_trimpoints(const char *path, uint32_t *start, uint32_t *end);

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
 * \param[in] path The path string to traverse. Must be null terminated.
 * \param[in] start_index The index to start traversing from. -1 means start from the end.
 * \return Index of last path separator. -1 if there are no separators left.
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

#endif /* _KIOKU_FILESYSTEM_H */
