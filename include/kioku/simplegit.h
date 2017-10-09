#ifndef _KIOKU_SIMPLEGIT_H
#define _KIOKU_SIMPLEGIT_H
#include "git2.h"

typedef struct _kioku_repo_init_options {
  const char *first_file_name;
  const char *first_file_content;
  const char *first_commit_message;
} kioku_repo_init_options;

#endif /* _KIOKU_SIMPLEGIT_H */
