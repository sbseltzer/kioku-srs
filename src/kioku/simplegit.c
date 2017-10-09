#include "kioku/simplegit.h"
#include "kioku/filesystem.h"

bool kioku_simplegit_init()
{
  git_libgit2_init();
  return true;
}

void kioku_simplegit_exit()
{
  git_libgit2_shutdown();
}

bool kioku_simplegit_isrepo(const char *path)
{
  return false;
}

bool kioku_simplegit_repo_new(git_repository **repo_out, const char *path, const kioku_repo_init_options opts)
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

bool kioku_simplegit_repo_commit(git_respository *repo, const char *message)
{
  
}

bool kioku_simplegit_repo_add(git_respository *repo, const char *path)
{
  
}

bool kioku_simplegit_repo_clone(git_repository **repo_out, const char *path, const char *remote_url)
{
  bool result = false;
  git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
  git_clone(repo_out, remote_url, path, &opts);
  return result;
}
