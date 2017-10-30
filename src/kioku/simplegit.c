#include "git2.h"
#include "kioku/simplegit.h"
#include "kioku/filesystem.h"
#include "kioku/log.h"
#include <string.h>

static git_repository *srsGIT_REPO = NULL;
static int srsGIT_READY = 0;
/** Useful resources:
 *  - https://libgit2.github.com/docs/guides/101-samples/
 *  - https://github.com/libgit2/libgit2/blob/master/examples/general.c
 *  - https://stackoverflow.com/questions/27672722/libgit2-commit-example
 *  - https://git-scm.com/book/en/v2/Git-Internals-Plumbing-and-Porcelain
 *  - https://git-scm.com/book/be/v2/Appendix-B%3A-Embedding-Git-in-your-Applications-Libgit2
 */

#define srsGIT_INIT_LIB() assert((srsGIT_READY = git_libgit2_init()) > 0)
#define srsGIT_EXIT_LIB() assert((srsGIT_READY = git_libgit2_shutdown()) >= 0)

uint32_t srsGit_InitCount()
{
  return srsGIT_READY;
}

bool srsGit_Repo_Close()
{
  /* \todo The docs say that any objects associated with the freed repo will remain until freed, and accessing them without their backing repo will result in undefined behaviour. Come up with a strategy to ensure they are all freed here, or at least assert that there's nothing remaining (since not freeing them would be the fault of the programmer). */
  if (srsGIT_REPO != NULL)
  {
    git_repository_free(srsGIT_REPO);
    srsGIT_REPO = NULL;
  }
  return srsGIT_REPO == NULL;
}

bool srsGit_Shutdown()
{
  while (srsGit_InitCount() > 0)
  {
    srsGIT_EXIT_LIB();
  }
  return (srsGIT_READY == 0) && srsGit_Repo_Close();
}

bool srsGit_IsRepo(const char *path)
{
  /* Pass NULL for the output parameter to check for but not open the repo */
  bool result = false;
  srsGIT_INIT_LIB();
  git_repository *repo = NULL;
  int git_result = git_repository_open_ext(&repo, path, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
  result = (git_result == 0);
  git_repository_free(repo);
  srsGIT_EXIT_LIB();
  return result;
}

const char *srsGit_Repo_GetCurrent()
{
  const char *result = NULL;
  srsGIT_INIT_LIB();
  result = git_repository_path(srsGIT_REPO);
  srsGIT_EXIT_LIB();
  return result;
}

bool srsGit_Repo_Open(const char *path)
{
  bool result = true;
  srsGIT_INIT_LIB();

  /* Free the current repository first, if any */
  const char *currepo = srsGit_Repo_GetCurrent();
  if (currepo != NULL)
  {
    printf("Closing out %s before opening %s" kiokuSTRING_LF, currepo, path);
    result = srsGit_Repo_Close();
    if (result)
    {
      printf("Closed repo successfully.");
    }
    else
    {
      fprintf(stderr, "Something went wrong while trying to free the repo. Opening a new one could be dangerous!");
    }
  }
  if (!result)
  {
    return result;
  }

  int git_result = git_repository_open(&srsGIT_REPO, path);
  result = (srsGIT_REPO != NULL);
  /* We do not call srsGIT_EXIT_LIB here because we want the user to be able to continue using the repository */
  return result;
}

#if 0
bool srsGit_Repo_Clone(const char *path, const char *remote_url)
{
  bool result = false;
  git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
  srsGIT_INIT_LIB();
  int git_result = git_clone(&srsGIT_REPO, remote_url, path, &opts);
  /* \todo handle errors */
  result = git_result == GIT_OK;
  srsGIT_EXIT_LIB();
  return result;
}
#endif

bool srsGit_Repo_Create(const char *path, const srsGIT_CREATE_OPTS opts)
{
  bool result = false;
  char *fullpath = NULL;
  git_repository_init_options gitinitopts = GIT_REPOSITORY_INIT_OPTIONS_INIT;

  srsGIT_INIT_LIB();

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
    result = kioku_filesystem_setcontent(fullpath, opts.first_file_content);
  }
  if (result)
  {
    printf("Running add...\n");
    result = srsGit_Add(fullpath);
  }
  if (result)
  {
    printf("Running commit...\n");
    result = srsGit_Commit(opts.first_commit_message);
  }
  result = srsGit_Add(fullpath);
  free(fullpath);

  /* We do not call srsGIT_EXIT_LIB here because we want the user to be able to continue using the repository */

  return result;
}

bool srsGit_Commit(const char *message)
{
  int git_result = GIT_OK;
  bool result = true;
	git_oid oid, parent_id, commit_id;
	git_tree *tree;
	git_commit *parent;
	char oid_hex[GIT_OID_HEXSZ+1] = { 0 };
	git_index *index;
  srsGIT_INIT_LIB();
  if (srsGIT_REPO == NULL)
  {
    return false;
  }

  /* Try to open the index */
  git_result = git_repository_index(&index, srsGIT_REPO);
  result = result && (git_result == 0);
  if (!result)
  {
    const git_error *err = giterr_last();
    if (err == NULL)
    {
      fprintf(stderr, "Error occurred, but giterr_last returned null...\n");
      abort();
    }
    fprintf(stderr, "Could not open repository index: %s\n", err->message);
    abort();
  }

  /* See if this is the first commit */
  git_result = git_repository_head_unborn(srsGIT_REPO);
  result = result && (git_result == 0 || git_result == 1);
  if (git_result == 1)
  {
    git_result = git_index_write_tree(&oid, index);
    result = result && (git_result == 0);
    if (!result)
    {
      fprintf(stderr, "Unable to write initial tree from index\n");
      abort();
    }
  }

  size_t count = git_index_entrycount(index);
  /* \todo On windows VC %zu doesn't seem to be valid. Wrap or define the PRIs for it. */
  printf("Index entry count: %zu\n", count);
	printf("\n*Commit Writing*\n");

	/**
	 * Creating signatures for an authoring identity and time is simple.  You
	 * will need to do this to specify who created a commit and when.  Default
	 * values for the name and email should be found in the `user.name` and
	 * `user.email` configuration options.  See the `config` section of this
	 * example file to see how to access config values.
	 */
  git_signature *me = NULL;
  git_result = git_signature_now(&me, "Me", "me@example.com");
  result = result && (git_result == 0);
  if (!result)
  {
    fprintf(stderr, "failed to create signature\n");
    abort();
  }

	/**
	 * Commit objects need a tree to point to and optionally one or more
	 * parents.  Here we're creating oid objects to create the commit with,
	 * but you can also use
	 */

  /* git_result = git_reference_name_to_id(&oid, srsGIT_REPO, "HEAD"); */
	git_result = git_tree_lookup(&tree, srsGIT_REPO, &oid);
  result = result && (git_result == 0);
  if (!result)
  {
    fprintf(stderr, "failed to lookup tree\n");
    abort();
  }
  git_result = git_repository_head_unborn(srsGIT_REPO);
  result = result && (git_result == 0 || git_result == 1);
  if (git_result == 1)
  {
    parent = NULL;
  }
  else if (git_result == 0)
  {
    git_result = git_commit_lookup(&parent, srsGIT_REPO, &oid);
    result = result && (git_result == 0);
    if (!result)
    {
      const git_error *err = giterr_last();
      if (err == NULL)
      {
        fprintf(stderr, "Error occurred, but giterr_last returned null...\n");
      }
      fprintf(stderr, "Could not lookup parent commit: %s\n", err->message);
      abort();
    }
  }
  else
  {
    fprintf(stderr, "Failed to check whether unborn\n");
    abort();
  }

  const git_commit *parents[] = {parent};
	/**
	 * Here we actually create the commit object with a single call with all
	 * the values we need to create the commit.  The SHA key is written to the
	 * `commit_id` variable here.
	 */
	git_result = git_commit_create(
    &commit_id, /* out id */
    srsGIT_REPO,
    "HEAD", /* name of ref to update */
    me, me, /* author & committer */
    "UTF-8",
    message,
    tree,
    parent == NULL ? 0 : 1, parents);

  result = result && (git_result == GIT_OK);
  if (!result)
  {
    fprintf(stderr, "failed to lookup parent commit\n");
    abort();
  }

	/**
	 * Now we can take a look at the commit SHA we've generated.
	 */
	git_oid_fmt(oid_hex, &commit_id);
  printf("New Commit: %s\n", oid_hex);

  /* Cleanup */
	git_tree_free(tree);
  git_index_free(index);
  if (parent != NULL)
	{
    git_commit_free(parent);
  }
	git_signature_free(me);

  srsGIT_EXIT_LIB();

  return result;
}

bool srsGit_Add(const char *path)
{
  bool result = true;
  int git_result = 0;
	git_oid oid;
	git_tree *tree = NULL;
	git_index *index = NULL;

  if (srsGIT_REPO == NULL)
  {
    return false;
  }

  srsGIT_INIT_LIB();

  /* See if this is the first commit */
  git_result = git_repository_head_unborn(srsGIT_REPO);
  if (git_result == 1)
  {
    printf("Adding - Unborn HEAD\n");
  }
  git_result = git_repository_index(&index, srsGIT_REPO);
  result = result && (git_result == 0) && (index != NULL);
  if (!result)
  {
    fprintf(stderr, "Could not open repository index");
    abort();
  }
  git_result = git_index_write_tree(&oid, index);
  result = result && (git_result == 0);
  if (!result)
  {
    fprintf(stderr, "Unable to write initial tree from index");
    abort();
  }
  git_result = git_tree_lookup(&tree, srsGIT_REPO, &oid);
  result = result && (git_result == 0) && (tree != NULL);
  if (!result)
  {
    fprintf(stderr, "Unable to lookup tree from oid");
    abort();
  }

  const char *repo_path = srsGit_Repo_GetCurrent();
  char fullpath[kiokuPATH_MAX] = {0};
  size_t fullpath_len = kioku_path_getfull(path, fullpath, sizeof(fullpath));
  assert(fullpath_len > 0);
  size_t str_index = 0;
  printf("Checking %s against %s for relative path\n", fullpath, repo_path);
  if (strncmp(repo_path, fullpath, strlen(repo_path)) < 0)
  {
    while ((repo_path[str_index] == fullpath[str_index]) && (str_index < fullpath_len))
    {
      str_index++;
    }
  }
  printf("Adding %s to %s\n", &fullpath[str_index], repo_path);
  git_result = git_index_add_bypath(index, &fullpath[str_index]);
  printf("Inspecting result\n");
  result = result && (git_result == 0);
  if (!result)
  {
    fprintf(stderr, "Unable to add %s to %s\n", &fullpath[str_index], repo_path);
  }
  size_t count = git_index_entrycount(index);
  printf("Index entry count: %zu\n", count);
  /* Write the index so it doesn't show our added entry as untracked */
  git_index_write(index);
  git_index_free(index);
	git_tree_free(tree);

  assert(count == 1);
  srsGIT_EXIT_LIB();

  return result;
}

