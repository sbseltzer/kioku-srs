#ifndef _KIOKU_MODEL_H
#define _KIOKU_MODEL_H

#include "kioku/types.h"
#include "kioku/log.h"
#include "kioku/filesystem.h"

#ifndef KIOKU_MODEL_USERLIST_NAME
#define KIOKU_MODEL_USERLIST_NAME "users.json"
#endif

#define srsMODEL_CARD_ID_MAX 256

kiokuAPI bool srsModel_Card_GetNextID(const char *deck_path, char *card_id_buf, size_t card_id_buf_size);

bool kioku_model_init(uint32_t argc, char **argv);
void kioku_model_exit();

uint32_t kioku_get_deck_count();
uint32_t kioku_get_deck_names(char);

/**
 * Creates an empty deck with the specified name on the filesystem and commits it.
 * Returns a deck handle which must later be freed.
 */
bool kioku_deck_create(const char *name);
bool kioku_deck_commit();
bool kioku_deck_delete();

#endif /* _KIOKU_MODEL_H */
