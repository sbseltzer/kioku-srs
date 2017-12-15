/**
 * @addtogroup Model
 *
 * Manages the Kioku data model.
 *
 * @{
 */

#ifndef _KIOKU_MODEL_H
#define _KIOKU_MODEL_H

#include "kioku/decl.h"
#include "kioku/types.h"
#include "kioku/result.h"

#ifndef KIOKU_MODEL_USERLIST_NAME
#define KIOKU_MODEL_USERLIST_NAME "users.json"
#endif

#define srsMODEL_CARD_ID_MAX 256
#define srsMODEL_DECK_ID_MAX 256

/**
 * Set the root path for all model operations. All non-absolute paths passed to the model API are assumed to be relative to it.
 * @param[in] path Path to use as model root. If NULL, it will attempt to close out any resources associated with it. Otherwise, it must be an existing directory that is also a git repository. The string is duplicated - no reference to the actual pointer is kept.
 * @return srsOK or an error.
 */
kiokuAPI srsRESULT srsModel_SetRoot(const char *path);

/**
 * Get the root path for all model operations. It will be an absolute path.
 * @param[in] path Path to use as model root. Must be an existing directory that is also a git repository. The string is duplicated - no reference to the actual pointer is kept.
 * @return The current model root path or NULL if one is not set.
 */
kiokuAPI const char *srsModel_GetRoot();

/**
 * Check to see if the path is to a valid model root directory.
 * This is only true if it's a workdir of a git repository and contains known metadata files that are characteristic of the model.
 * @param[in] path An absolute or relative path. This is relative to the CWD.
 * @return True if it's a valid root, false if it's not. Error data will be set to indicate why it's invalid.
 */
kiokuAPI bool srsModel_IsValidRoot(const char *path);

/**
 * See if a valid filesystem entity is inside the model root. Values that resolve to the model root do not count as "in" it. Used primarily for input validation.
 * Once the model root is set, the model API ignores current working directory, doing everything from the model root.
 * That includes relative path input to this function.
 * @param[in] path An absolute or relative path. Relative paths are counted as relative to the root, NOT the current working directory.
 * @return True if it's within the root path, false if it's not. If root path is not set, it will return false with error data set.
 */
kiokuAPI bool srsModel_ExistsInRoot(const char *path);

/**
 * Attempt to create an empty model root.
 * @param[in] path An absolute or relative path. This is relative to the CWD.
 * @return srsOK if it succeeds.
 */
kiokuAPI srsRESULT srsModel_CreateRoot(const char *path);

/**
 * Attempt to create an empty model root and then set it. Shorthand for calling @ref srsModel_CreateRoot followed by @ref srsModel_SetRoot.
 * @param[in] path An absolute or relative path. This is relative to the CWD.
 * @return srsOK if it succeeds.
 */
kiokuAPI srsRESULT srsModel_CreateAndSetRoot(const char *path);

/**
 * Get the next card ID for a deck.
 * @param[in] deck_path Path to the deck to get the next card ID of.
 * @param[out] card_id_out Place to store the ID string. Must be large enough to include a null-terminator.
 * @param[in] card_id_out_size Size of the buffer, including the null terminator.
 * @return Whether the card ID could be stored. False could indicate a bad parameter, an invalid deck, a deck with no next card, or a buffer that's not large enough according to the specified size. @todo Failure ambiguity is a design flaw. For now the user is expected to use @ref srsMODEL_CARD_ID_MAX to protect themselves.
 */
kiokuAPI bool srsModel_Card_GetNextID(const char *deck_path, char *card_id_out, size_t card_id_out_size);

/**
 * Outputs the card path to the specified card ID within the specified deck. Returns false if it fails to find it for whatever reason.
 * @param[in] deck_path Path to the deck to look in.
 * @param[in] card_id The ID of the card to get the content of.
 * @param[out] path_out Buffer to store the the path in, including null-terminator. The contents are unspecified if this function returns false.
 * @param[in] path_size The size of the @ref path_out buffer including the null-terminator. If there's not enough room, this function will return false.
 * @return Whether the path could be stored. False could indicate a bad parameter, an invalid deck, an invalid card ID, or a buffer that's not large enough according to the specified size. @todo Failure ambiguity is a design flaw. For now the user is expected to use @ref srsPATH_MAX + 1 to protect themselves.
 */
kiokuAPI bool srsModel_Card_GetPath(const char *deck_path, const char *card_id, char *path_out, size_t path_size);

/**
 * Outputs the content of the next scheduled card in the specified deck path.
 * @param[in] deck_path Path to the deck to look in.
 * @param[in] card_id The ID of the card to get the content of. It will only be considered valid if a file @ref srsModel_Card_GetPath gives back an existing path.
 * @param[out] buf Buffer to store the content in, if any. The contents are unspecified if this function returns false.
 * @param[in] buf_size Size of the buffer to store the ID in, including the null-terminator. To ensure there's enough room, use the length of the file content plus one.
 * @return True if a the content is available for the specified deck/card ID, and the content size did not exceed the provided buffer size. @todo Failure ambiguity is a design flaw. For now the user should use @ref srsFile_GetLength to protect themselves.
 */
kiokuAPI bool srsModel_Card_GetCardContent(const char *deck_path, const char *card_id, const char *buf, size_t buf_size);

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
