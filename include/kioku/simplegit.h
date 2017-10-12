#ifndef _KIOKU_SIMPLEGIT_H
#define _KIOKU_SIMPLEGIT_H

#include <kioku/decl.h>
#include <kioku/types.h>
#include <kioku/debug.h>

typedef struct srsGIT_CREATE_OPTS_s {
  const char *first_file_name;
  const char *first_file_content;
  const char *first_commit_message;
} srsGIT_CREATE_OPTS;

#define srsGIT_CREATE_OPTS_DEFAULT {".gitignore", "", "Initial Commit"}

/** Create a repository with a first file and initial commit.
 * This will update all git functions to operate on the new repository.
 * \param[in] path The path at which to create it, which represents a directory, and serve as the home of the .git folder.
 * \param[in] opts The options that specify how the repository is initialized. If NULL, it will be substituted with srsGIT_CREATE_OPTS_DEFAULT. If non-NULL but with some NULL parameters, the NULL parameters will be substituted with the corresponding value in srsGIT_CREATE_OPTS_DEFAULT.
 * \return Whether creation was successful.
 */
kiokuAPI bool srsGit_Repo_Create(const char *path, const srsGIT_CREATE_OPTS opts);

/** Return the number of times the git library has been initialized.
 * If it's greater than 1, a git shutdown is needed.
 * \return Whether git is still initialized, and if so, how many times.
 */
kiokuAPI uint32_t srsGit_InitCount();

/** Clean up the currently open repo.
 * \return Whether a repository was closed.
 */
kiokuAPI bool srsGit_Repo_Close();

/** Fully shut down all inits and free/nullify the working repo.
 * \return Whether it was successful.
 */
kiokuAPI bool srsGit_Shutdown();

kiokuAPI bool srsGit_Commit(const char *path);

kiokuAPI bool srsGit_Add(const char *message);

kiokuAPI bool srsGit_IsRepo(const char *path);
#endif /* _KIOKU_SIMPLEGIT_H */
