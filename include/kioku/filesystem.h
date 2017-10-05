#ifndef _KIOKU_FILESYSTEM_H
#define _KIOKU_FILESYSTEM_H

#include "kioku/types.h"
#include "kioku/log.h"
#include "string.h"

#define KIOKU_DIRSEP "/"

int32_t kioku_path_concat(char *dest, size_t destsize, const char *path1, const char *path2);

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
 */

const char *kioku_get_conf_directory();
const char *kioku_get_user_directory(const char *username);

bool kioku_file_create(const char *path);
bool kioku_file_rename(const char *path, const char *newpath);
bool kioku_file_delete(const char *path);
bool kioku_file_exists(const char *path);

bool kioku_directory_create(const char *path);
bool kioku_directory_rename(const char *path, const char *newpath);
bool kioku_directory_delete(const char *path);
bool kioku_directory_exists(const char *path);

#endif /* _KIOKU_FILESYSTEM_H */
