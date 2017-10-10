#include "git2.h"
#include "kioku/simplegit.h"
#include "kioku/filesystem.h"
#include "kioku/log.h"

static git_repository *srsGIT_REPO = NULL;
static bool srsGIT_READY = false;
/** Useful resources:
 *  - https://libgit2.github.com/docs/guides/101-samples/
 *  - https://github.com/libgit2/libgit2/blob/master/examples/general.c
 *  - https://stackoverflow.com/questions/27672722/libgit2-commit-example
 *  - https://git-scm.com/book/en/v2/Git-Internals-Plumbing-and-Porcelain
 *  - https://git-scm.com/book/be/v2/Appendix-B%3A-Embedding-Git-in-your-Applications-Libgit2
 */
bool srsGit_Init()
{
  int num_inits = git_libgit2_init();
  srsGIT_READY = num_inits > 0;
  return srsGIT_READY;
}

bool srsGit_Exit()
{
  int inits_left = 0;
  do {
    inits_left = git_libgit2_shutdown();
  } while (inits_left > 0);
  srsGIT_READY = (inits_left == 0);
  if (inits_left < GIT_OK)
  {
    /* \todo handle error */
  }
  return !srsGIT_READY;
}

bool srsGit_IsRepo(const char *path)
{
  /* Pass NULL for the output parameter to check for but not open the repo */
  return (git_repository_open_ext(NULL, path, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL) == 0);
}

const char *srsGit_Repo_GetCurrent()
{
  return git_repository_path(srsGIT_REPO);
}

bool srsGit_Repo_Open(const char *path)
{
  bool result = false;
  int error = git_repository_open(&srsGIT_REPO, path);
  result = error == GIT_OK;
  /* \todo handle errors */
  return result;
}

bool srsGit_Repo_Clone(const char *path, const char *remote_url)
{
  bool result = false;
  git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
  int error = git_clone(&srsGIT_REPO, remote_url, path, &opts);
  /* \todo handle errors */
  result = error == GIT_OK;
  return result;
}

bool srsGit_Repo_Create(const char *path, const srsGIT_CREATE_OPTS opts)
{
  bool result = false;
  char *fullpath = NULL;
  git_repository_init_options gitinitopts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
  gitinitopts.flags = GIT_REPOSITORY_INIT_MKPATH;
  git_repository_init_ext(&srsGIT_REPO, path, &gitinitopts);

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
    result = srsGit_Add(fullpath);
  }
  free(fullpath);
  if (result)
  {
    result = srsGitCommit(opts.first_commit_message);
  }
  return result;
}

bool srsGit_Commit(const char *message)
{
  git_oid oid = 0;
  int error = GIT_OK;

  error = git_reference_name_to_id(&oid, srsGIT_REPO, "HEAD");

  git_commit *parent = NULL;
  error = git_commit_lookup(&parent, srsGIT_REPO, git_object_id(obj));

  git_signature *me = NULL;
  error = git_signature_now(&me, "Me", "me@example.com");

  const git_commit *parents[] = {parent};

  git_oid new_commit_id = 0;
  error = git_commit_create(
    &new_commit_id,
    srsGIT_REPO,
    "HEAD",                      /* name of ref to update */
    me,                          /* author */
    me,                          /* committer */
    "UTF-8",                     /* message encoding */
    message,  /* message */
    tree,                        /* root tree */
    2,                           /* parent count */
    parents);                    /* parents */
}

bool srsGit_Add(const char *path)
{
  int error;
  git_index *idx = NULL;
  error = git_repository_index(&idx, srsGIT_REPO);
  error = git_index_add_bypath(idx, path);
}

