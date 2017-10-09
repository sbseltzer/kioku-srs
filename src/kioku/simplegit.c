#include "kioku/simplegit.h"
#include "kioku/filesystem.h"

/** Useful resources:
 *  - https://libgit2.github.com/docs/guides/101-samples/
 *  - https://github.com/libgit2/libgit2/blob/master/examples/general.c
 *  - https://stackoverflow.com/questions/27672722/libgit2-commit-example
 *  - https://git-scm.com/book/en/v2/Git-Internals-Plumbing-and-Porcelain
 */
bool kioku_simplegit_init()
{
  git_libgit2_init();
  return true;
}

void kioku_simplegit_exit()
{
  git_libgit2_shutdown();
}

bool srsGit_IsRepo(const char *path)
{
  /* Pass NULL for the output parameter to check for but not open the repo */
  return (git_repository_open_ext(NULL, path, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL) == 0);
}

bool srsGit_Repo_Open(git_repository **repo_out, const char *path, const kioku_repo_init_options opts)
{

}
bool srsGit_Repo_Clone(git_repository **repo_out, const char *path, const char *remote_url)
{
  bool result = false;
  git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
  git_clone(repo_out, remote_url, path, &opts);
  return result;
}
bool srsGit_Repo_Create(git_repository **repo_out, const char *path, const kioku_repo_init_options opts)
{
  bool result = false;
  char *fullpath = NULL;
  git_repository_init_options gitinitopts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
  gitinitopts.flags = GIT_REPOSITORY_INIT_MKPATH;
  git_repository_init_ext(repo_out, path, &gitinitopts);

  int32_t pathlen = kioku_path_concat(NULL, 0, path, opts.first_file_name);
  if (pathlen <= 0)
  {
    return result;
  }
  fullpath = malloc(pathlen + 1);
  int32_t wrote = kioku_path_concat(fullpath, pathlen + 1, path, opts.first_file_name);
  result = (wrote == pathlen);
  if (result)
  {
    result = kioku_filesystem_create(fullpath);
  }
  if (result)
  {
    result = kioku_simplegit_repo_add(fullpath);
  }
  free(fullpath);
  if (result)
  {
    result = kioku_simplegit_repo_commit(*repo_out, opts.first_commit_message);
  }
  return result;
}

bool srsGit_Commit(git_respository *repo, const char *message)
{
  git_oid oid = 0;
  int error = GIT_SUCCESS;

  error = git_reference_name_to_id(&oid, repo, "HEAD");

  git_commit *parent = NULL;
  error = git_commit_lookup(&parent, repo, git_object_id(obj));

  git_signature *me = NULL;
  error = git_signature_now(&me, "Me", "me@example.com");

  const git_commit *parents[] = {parent};

  git_oid new_commit_id = 0;
  error = git_commit_create(
    &new_commit_id,
    repo,
    "HEAD",                      /* name of ref to update */
    me,                          /* author */
    me,                          /* committer */
    "UTF-8",                     /* message encoding */
    message,  /* message */
    tree,                        /* root tree */
    2,                           /* parent count */
    parents);                    /* parents */


}

bool srsGit_Add(git_respository *repo, const char *path)
{
  int error;
  git_index *idx = NULL;
  error = git_repository_index(&idx, repo);
  error = git_index_add_bypath(idx, path);
}

