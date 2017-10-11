#ifndef _KIOKU_SIMPLEGIT_H
#define _KIOKU_SIMPLEGIT_H

typedef struct srsGIT_CREATE_OPTS_s {
  const char *first_file_name;
  const char *first_file_content;
  const char *first_commit_message;
} srsGIT_CREATE_OPTS;

#define srsGIT_CREATE_OPTS_DEFAULT {".gitignore", "", "Initial Commit"}

/** Create a repository with a first file and initial commit.
 * \param[in] path The path at which to create it, which represents a directory, and serve as the home of the .git folder.
 * \param[in] opts The options that specify how the repository is initialized. If NULL, it will be substituted with srsGIT_CREATE_OPTS_DEFAULT. If non-NULL but with some NULL parameters, the NULL parameters will be substituted with the corresponding value in srsGIT_CREATE_OPTS_DEFAULT.
 * \return Whether creation was successful.
 */
kiokuAPI bool srsGit_Repo_Create(const char *path, const srsGIT_CREATE_OPTS opts);

#endif /* _KIOKU_SIMPLEGIT_H */
