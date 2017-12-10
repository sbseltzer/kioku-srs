#ifndef _KIOKU_SIMPLEGIT_H
#define _KIOKU_SIMPLEGIT_H

#include <kioku/decl.h>
#include <kioku/types.h>
#include <kioku/result.h>
#include <kioku/debug.h>

typedef struct srsGIT_CREATE_OPTS_s {
  const char *first_file_name; /** This will be relative to the new repository */
  const char *first_file_content;
  const char *first_commit_message;
} srsGIT_CREATE_OPTS;

#define srsGIT_CREATE_OPTS_INIT (srsGIT_CREATE_OPTS){".gitignore", "", "Initial Commit"}

/**
 * Create a repository with a first file and initial commit.
 * This will update all git functions to operate on the new repository.
 * @param[in] path The path at which to create it, which represents a directory, and serve as the home of the .git folder. This can be a relative path.
 * @param[in] opts The options that specify how the repository is initialized. If NULL, it will be substituted with srsGIT_CREATE_OPTS_INIT. If non-NULL but with some NULL parameters, the NULL parameters will be substituted with the corresponding value in srsGIT_CREATE_OPTS_INIT.
 * @return Whether creation was successful.
 */
kiokuAPI bool srsGit_Repo_Create(const char *path, const srsGIT_CREATE_OPTS opts);

/**
 * Return the number of times the git library has been initialized.
 * If it's greater than 1, a git shutdown is needed.
 * @return Whether git is still initialized, and if so, how many times.
 */
kiokuAPI uint32_t srsGit_InitCount();

/**
 * Clean up the currently open repo (if any).
 */
kiokuAPI void srsGit_Repo_Close();

/**
 * Fully shut down all inits and free/nullify the working repo.
 * @return Whether it was successful.
 */
kiokuAPI bool srsGit_Shutdown();

/**
 * Commit whatever has been added.
 * @param[in] message The commit message
 * @return Whether it was successful.
 */
kiokuAPI bool srsGit_Commit(const char *message);

/**
 * Add a path to be committed.
 * @param[in] path The path(s) to be added. This can be a regex search for files to add. They must be relative to the root of the repository.
 * @return Whether it was successful.
 */
kiokuAPI bool srsGit_Add(const char *path);

/**
 * Whether a path represents a valid git repository.
 * @param[in] path The path to the repository.
 * @return Whether it is a repository
 */
kiokuAPI bool srsGit_IsRepo(const char *path);

/**
 * Set the new current repository to work with.
 * @param[in] path The path to the repository.
 * @return srsOK if successful
 */
kiokuAPI srsRESULT srsGit_Repo_Open(const char *path);

/**
 * Get the current repo path
 * @return Current repo path. Memory is owned by the implementation.
 */
kiokuAPI const char *srsGit_Repo_GetCurrent();

#endif /* _KIOKU_SIMPLEGIT_H */
