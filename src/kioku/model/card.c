#include "kioku/model/card.h"
#include "kioku/filesystem.h"
#include "kioku/string.h"

/**
 * Returns a list of all cards for a deck.
 * TODO Some day we need to support decks with thousands of cards. We cannot allocate that much memory, and we have no real reason to. We will need a new function to replace this one that allows us to filter them.
 * @param[in] deck_path Path to the deck to get the cards from.
 * @param[out] count_out Place to store the number of elements in the returned array.
 * @return Unmanaged dynamically allocated card array, or NULL if no cards are found.
 */
srsCARD *srsCard_GetAll(const char *deck_path, size_t *count_out)
{
  return NULL;
}

/**
 * Returns the content of a file associated with the specified card.
 * @param[in] card The card.
 * @param[in] file The filename to find relative to the card's directory.
 * @return Unmanaged dynamically allocated text of a file for the specified card. Returns NULL if the file doesn't exist relative to the card directory.
 */
char *srsCard_GetContent(srsCARD card, const char *file)
{
  return NULL;
}
