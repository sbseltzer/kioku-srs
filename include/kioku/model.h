/**
 * @addtogroup Model
 *
 * Manages the Kioku data model.
 *
 * @{
 */

#ifndef _KIOKU_MODEL_H
#define _KIOKU_MODEL_H

#include "kioku/types.h"
#include "kioku/log.h"
#include "kioku/filesystem.h"

#ifndef KIOKU_MODEL_USERLIST_NAME
#define KIOKU_MODEL_USERLIST_NAME "users.json"
#endif

#define srsMODEL_CARD_ID_MAX 256

/**
 * Get the next card ID for a deck.
 * @param[in] deck_path Path to the deck to get the next card ID of.
 * @param[out] card_id_buf Place to store the ID string. Must be large enough to include a null-terminator.
 * @param[in] card_id_buf_size Size of the buffer, including the null terminator.
 * @return Whether the card ID could be stored. False could indicate a bad parameter, an invalid deck, a deck with no next card, or a buffer that's not large enough according to the specified size.
 */
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

/** @} */
