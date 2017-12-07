#include "git2.h"
#include "kioku/git.h"
#include "kioku/filesystem.h"
#include "kioku/log.h"
#include "kioku/result.h"
#include "kioku/error.h"
#include <string.h>

static git_repository *srsGit_REPO = NULL;
static char *srsGit_REPO_PATH = NULL;
static int srsGIT_READY = 0;
/** Useful resources:
 *  - https://libgit2.github.com/docs/guides/101-samples/
 *  - https://github.com/libgit2/libgit2/blob/master/examples/general.c
 *  - https://stackoverflow.com/questions/27672722/libgit2-commit-example
 *  - https://git-scm.com/book/en/v2/Git-Internals-Plumbing-and-Porcelain
 *  - https://git-scm.com/book/be/v2/Appendix-B%3A-Embedding-Git-in-your-Applications-Libgit2
 */

static uint32_t srsGIT_INIT_LIB()
{
  uint32_t result = (srsGIT_READY = git_libgit2_init());
  assert(result > 0);
  return result;
}
static uint32_t srsGIT_EXIT_LIB()
{
  uint32_t result = (srsGIT_READY = git_libgit2_shutdown());
  assert(result >= 0);
  return result;
}
static void srsGIT_DEBUG_ERROR()
{
  const git_error *err = giterr_last();
  if (err == NULL)
  {
    srsLOG_ERROR("Error occurred, but giterr_last returned null...");
    abort();
  }
  srsLOG_ERROR("Git Error %d: %s", err->klass, err->message);
  abort();
}

uint32_t srsGit_InitCount()
{
  return srsGIT_READY;
}

void srsGit_Repo_Close()
{
  /* \todo The docs say that any objects associated with the freed repo will remain until freed, and accessing them without their backing repo will result in undefined behaviour. Come up with a strategy to ensure they are all freed here, or at least assert that there's nothing remaining (since not freeing them would be the fault of the programmer). */
  if (srsGit_REPO != NULL)
  {
    git_repository_free(srsGit_REPO);
    srsGit_REPO = NULL;
  }
  free(srsGit_REPO_PATH);
  srsGit_REPO_PATH = NULL;
}

bool srsGit_Shutdown()
{
  srsGit_Repo_Close();
  while (srsGit_InitCount() > 0)
  {
    srsGIT_EXIT_LIB();
  }
  return (srsGIT_READY == 0);
}

bool srsGit_IsRepo(const char *path)
{
  /* Pass NULL for the output parameter to check for but not open the repo */
  bool result = false;
  srsGIT_INIT_LIB();
  git_repository *repo = NULL;
  int git_result = git_repository_open_ext(&repo, path, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
  result = (git_result == 0);
  if (!result && (git_result != GIT_ENOTFOUND))
  {
    srsGIT_DEBUG_ERROR();
  }
  if (repo != NULL)
  {
    git_repository_free(repo);
  }
  srsGIT_EXIT_LIB();
  return result;
}

const char *srsGit_Repo_GetCurrent()
{
  if (srsGit_REPO != NULL)
  {
    if (srsGit_REPO_PATH == NULL)
    {
      srsGIT_INIT_LIB();
      const char *workdir = git_repository_workdir(srsGit_REPO);
      srsGIT_EXIT_LIB();
      srsGit_REPO_PATH = strdup(workdir);
    }
  }
  return srsGit_REPO_PATH;
}

srsRESULT srsGit_Repo_Open(const char *path)
{
  srsGIT_INIT_LIB();
  /* Free the current repository first, if any */
  const char *currepo = srsGit_Repo_GetCurrent();
  if (currepo != NULL)
  {
    srsLOG_PRINT("Closing out %s before opening %s", currepo, path);
    srsGit_Repo_Close();
  }
  srsASSERT(srsGit_REPO == NULL);
  srsASSERT(srsGit_REPO_PATH == NULL);

  int git_result = git_repository_open(&srsGit_REPO, path);
  if (git_result != 0)
  {
    return srsFAIL;
  }
  /* We do not call srsGIT_EXIT_LIB here because we want the user to be able to continue using the repository */
  return srsOK;
}

#if 0
bool srsGit_Repo_Clone(const char *path, const char *remote_url)
{
  bool result = false;
  git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
  srsGIT_INIT_LIB();
  int git_result = git_clone(&srsGit_REPO, remote_url, path, &opts);
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
  /* Replace opts accordingly */
  /** TODO Test defaulting of opts */
  srsGIT_CREATE_OPTS opts_default = srsGIT_CREATE_OPTS_DEFAULT;
  srsGIT_CREATE_OPTS opts_copy = opts;
  opts_copy.first_file_name = opts_copy.first_file_name ? opts_copy.first_file_name : opts_default.first_file_name;
  opts_copy.first_commit_message = opts_copy.first_commit_message ? opts_copy.first_commit_message : opts_default.first_commit_message;
  opts_copy.first_file_content = opts_copy.first_file_content ? opts_copy.first_file_content : opts_default.first_file_content;

  git_repository_init_options gitinitopts = GIT_REPOSITORY_INIT_OPTIONS_INIT;

  srsGIT_INIT_LIB();

  gitinitopts.flags = GIT_REPOSITORY_INIT_MKPATH;
  int git_result = git_repository_init_ext(&srsGit_REPO, path, &gitinitopts);
  if (git_result != 0)
  {
    srsGIT_DEBUG_ERROR();
    srsGIT_EXIT_LIB();
    return result;
  }

  int32_t pathlen = kioku_path_concat(NULL, 0, path, opts_copy.first_file_name);
  if (pathlen <= 0)
  {
    srsLOG_ERROR("Couldn't calculate pathlength for %s", opts_copy.first_file_name);
    srsGIT_EXIT_LIB();
    return result;
  }
  fullpath = malloc(pathlen + 1);
  int32_t wrote = kioku_path_concat(fullpath, pathlen + 1, path, opts_copy.first_file_name);
  result = (wrote == pathlen);
  if (result)
  {
    result = srsFile_Create(fullpath);
  }
  if (result)
  {
    result = srsFile_SetContent(fullpath, opts_copy.first_file_content);
  }
  if (result)
  {
    srsLOG_PRINT("Running add...");
    result = srsGit_Add(opts_copy.first_file_name);
  }
  if (result)
  {
    srsLOG_PRINT("Running commit...");
    result = srsGit_Commit(opts_copy.first_commit_message);
  }
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
  if (srsGit_REPO == NULL)
  {
    return false;
  }

  srsGIT_INIT_LIB();

  /* Try to open the index */
  git_result = git_repository_index(&index, srsGit_REPO);
  result = result && (git_result == 0);
  if (!result)
  {
    srsGIT_DEBUG_ERROR();
    abort();
  }

  /* See if this is the first commit */
  git_result = git_repository_head_unborn(srsGit_REPO);
  result = result && (git_result == 0 || git_result == 1);
  if (git_result == 1)
  {
    git_result = git_index_write_tree(&oid, index);
    result = result && (git_result == 0);
    if (!result)
    {
      srsGIT_DEBUG_ERROR();
      srsLOG_ERROR("Unable to write initial tree from index");
      abort();
    }
  }

  size_t count = git_index_entrycount(index);
  /* \todo On windows VC %zu doesn't seem to be valid. Wrap or define the PRIs for it. */
  srsLOG_PRINT("Index entry count: %zu", count);
  srsLOG_PRINT("*Commit Writing*");

	/**
	 * Creating signatures for an authoring identity and time is simple.  You
	 * will need to do this to specify who created a commit and when.  Default
	 * values for the name and email should be found in the `user.name` and
	 * `user.email` configuration options.  See the `config` section of this
	 * example file to see how to access config values.
	 */
  /** TODO Don't use magic strings for these */
  git_signature *me = NULL;
  git_result = git_signature_now(&me, "Me", "me@example.com");
  result = result && (git_result == 0);
  if (!result)
  {
    srsLOG_ERROR("failed to create signature");
    abort();
  }

	/**
	 * Commit objects need a tree to point to and optionally one or more
	 * parents.  Here we're creating oid objects to create the commit with,
	 * but you can also use
	 */

  /* git_result = git_reference_name_to_id(&oid, srsGit_REPO, "HEAD"); */
	git_result = git_tree_lookup(&tree, srsGit_REPO, &oid);
  result = result && (git_result == 0);
  if (!result)
  {
    srsLOG_ERROR("failed to lookup tree");
    abort();
  }
  git_result = git_repository_head_unborn(srsGit_REPO);
  result = result && (git_result == 0 || git_result == 1);
  if (git_result == 1)
  {
    parent = NULL;
  }
  else if (git_result == 0)
  {
    git_result = git_commit_lookup(&parent, srsGit_REPO, &oid);
    result = result && (git_result == 0);
    if (!result)
    {
      const git_error *err = giterr_last();
      if (err == NULL)
      {
        srsLOG_ERROR("Error occurred, but giterr_last returned null...");
      }
      srsLOG_ERROR("Could not lookup parent commit: %s", err->message);
      abort();
    }
  }
  else
  {
    srsLOG_ERROR("Failed to check whether unborn");
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
    srsGit_REPO,
    "HEAD", /* name of ref to update */
    me, me, /* author & committer */
    "UTF-8",
    message,
    tree,
    parent == NULL ? 0 : 1, parents);

  result = result && (git_result == GIT_OK);
  if (!result)
  {
    srsLOG_ERROR("failed to lookup parent commit");
    abort();
  }

	/**
	 * Now we can take a look at the commit SHA we've generated.
	 */
	git_oid_fmt(oid_hex, &commit_id);
  srsLOG_PRINT("New Commit: %s", oid_hex);

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

  if (srsGit_REPO == NULL)
  {
    return false;
  }

  srsGIT_INIT_LIB();

  /* See if this is the first commit */
  git_result = git_repository_head_unborn(srsGit_REPO);
  if (git_result == 1)
  {
    srsLOG_PRINT("Adding - Unborn HEAD");
  }
  git_result = git_repository_index(&index, srsGit_REPO);
  result = result && (git_result == 0) && (index != NULL);
  if (!result)
  {
    srsLOG_ERROR("Could not open repository index");
    abort();
  }
  git_result = git_index_write_tree(&oid, index);
  result = result && (git_result == 0);
  if (!result)
  {
    srsLOG_ERROR("Unable to write initial tree from index");
    abort();
  }
  git_result = git_tree_lookup(&tree, srsGit_REPO, &oid);
  result = result && (git_result == 0) && (tree != NULL);
  if (!result)
  {
    srsLOG_ERROR("Unable to lookup tree from oid");
    abort();
  }

  const char *repo_path = srsGit_Repo_GetCurrent();
  srsLOG_PRINT("Adding %s to %s", path, repo_path);
  git_result = git_index_add_bypath(index, path);

  srsLOG_PRINT("Inspecting result");
  result = result && (git_result == 0);
  if (!result)
  {
    srsLOG_ERROR("Unable to add %s to %s", path, repo_path);
  }
  size_t count = git_index_entrycount(index);
  srsLOG_PRINT("Index entry count: %zu", count);
  /* Write the index so it doesn't show our added entry as untracked */
  git_index_write(index);
  git_index_free(index);
	git_tree_free(tree);

  assert(count == 1);
  srsGIT_EXIT_LIB();

  return result;
}

