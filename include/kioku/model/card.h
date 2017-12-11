#ifndef _KIOKU_MODEL_CARD_H
#define _KIOKU_MODEL_CARD_H

#include "kioku/schedule.h"
#include "kioku/result.h"

typedef struct _srsCARD_s
{
  const char *id;
  const char *path;
  srsTIME     when_added;
  srsTIME     when_next_scheduled;
} srsCARD;

/**
 * Returns a list of all cards for a deck.
 * TODO Some day we need to support decks with thousands of cards. We cannot allocate that much memory, and we have no real reason to. We will need a new function to replace this one that allows us to filter them.
 * @param[in] deck_name Name of the deck to get the cards from.
 * @param[out] count_out Place to store the number of elements in the returned array.
 * @return Unmanaged dynamically allocated card array, or NULL if no cards are found.
 */
kiokuAPI srsCARD *srsCard_GetAll(const char *deck_name, size_t *count_out);

/**
 * Returns the content of a file associated with the specified card.
 * @param[in] card The card.
 * @param[in] file The filename to find relative to the card's directory.
 * @return Unmanaged dynamically allocated text of a file for the specified card. Returns NULL if the file doesn't exist relative to the card directory.
 */
kiokuAPI char *srsCard_GetContent(srsCARD card, const char *file);

/**
 * Frees a card array returned by @ref srsCard_GetAll
 * @param[in] cards Array of cards
 * @param[in] count Number of cards
 * @return srsOK if it succeeded.
 */
kiokuAPI srsRESULT srsCard_FreeArray(srsCARD *cards, size_t count);

#endif /* _KIOKU_MODEL_CARD_H */
