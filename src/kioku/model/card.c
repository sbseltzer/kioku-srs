#include "kioku/model/card.h"
#include "kioku/filesystem.h"
#include "kioku/schedule.h"
#include "kioku/datastructure.h"
#include "kioku/string.h"
#include "kioku/debug.h"

static srsFILESYSTEM_VISIT_ACTION get_cards(const char *path, void *userdata)
{
  srsMEMSTACK *list = (srsMEMSTACK *)userdata;
  srsTIME_STRING added_time_str = {0};
  srsTIME_STRING scheduled_time_str = {0};
  /** TODO We should not rely on the push/pop/get CWD functions. It's not very robust. */
  /* Go into the card's folder */
  srsDir_PushCWD(path);
  {
    srsTIME added_time = srsTime_Now();
    srsTIME scheduled_time = srsTime_Now();
    bool ok = false;

    /* Try to load added time */
    ok = srsFile_GetContent("added.txt", added_time_str, sizeof(added_time_str));
    ok = srsTime_FromString(added_time_str, &added_time);
    if (!ok)
    {
      srsTime_ToString(added_time, added_time_str);
      srsFile_SetContent("added.txt", added_time_str);
    }
    /* Try to load scheduled time */
    ok = srsFile_GetContent("scheduled.txt", scheduled_time_str, sizeof(scheduled_time_str));
    ok = srsTime_FromString(scheduled_time_str, &scheduled_time);
    if (!ok)
    {
      srsTime_ToString(scheduled_time, scheduled_time_str);
      srsFile_SetContent("scheduled.txt", added_time_str);
    }

    /* Create card */
    srsCARD card = {0};
    card.id = strdup(path);
    /** TODO If we switch to a new filesystem iterator implementation, the srsDir_GetCWD usage here will be invalid */
    card.path = strdup(srsDir_GetCWD());
    srsASSERT(card.id != NULL);
    srsASSERT(card.path != NULL);
    card.when_added = added_time;
    card.when_next_scheduled = scheduled_time;
    /* Add to list */
    ok = srsMemStack_Push(list, &card);
  }
done:
  /* Go out of the card's folder */
  srsDir_PopCWD(NULL);
  return srsFILESYSTEM_VISIT_CONTINUE;
}

/**
 * Returns a list of all cards for a deck.
 * TODO Some day we need to support decks with thousands of cards. We cannot allocate that much memory, and we have no real reason to. We will need a new function to replace this one that allows us to filter them.
 * @param[in] deck_path Path to the deck to get the cards from.
 * @param[out] count_out Place to store the number of elements in the returned array.
 * @return Unmanaged dynamically allocated card array, or NULL if no cards are found.
 */
srsCARD *srsCard_GetAll(const char *deck_path, size_t *count_out)
{
  srsMEMSTACK list = {0};
  srsMemStack_Init(&list, sizeof(srsCARD), 16);

  srsDir_PushCWD(deck_path);
  bool ok = srsFileSystem_Iterate("cards", (void *)&list, get_cards);
  srsDir_PopCWD(NULL);

  *count_out = list.count;
  /* We can safely lose reference to the list since the internal malloc'd memory will still exist */
  return list.memory;
}

/**
 * Frees a card array returned by @ref srsCard_GetAll
 * @param[in] cards Array of cards
 * @param[in] count Number of cards
 * @return Whether it succeeded.
 */
srsRESULT srsCard_FreeArray(srsCARD *cards, size_t count)
{
  if (cards == NULL || count == 0)
  {
    return srsOK;
  }
  while (count > 0)
  {
    free(cards[count-1].id);
    free(cards[count-1].path);
    count--;
  }
  free(cards);
  return srsOK;
}

/**
 * Returns the content of a file associated with the specified card.
 * @param[in] card The card.
 * @param[in] file The filename to find relative to the card's directory.
 * @return Unmanaged dynamically allocated text of a file for the specified card. Returns NULL if the file doesn't exist relative to the card directory.
 */
char *srsCard_GetContent(srsCARD card, const char *file)
{
  bool ok = false;
  size_t content_size = 0;
  char *content = NULL;
  srsDir_PushCWD(card.path);
  content_size = srsFile_GetLength(file) + 1;
  if (content_size > 0)
  {
    goto done;
  }
  content = malloc(content_size);
  if (content == NULL)
  {
    goto done;
  }
  ok = srsFile_GetContent(file, content, content_size);
  if (!ok)
  {
    goto done;
  }
done:
  if (!ok)
  {
    free(content);
    content = NULL;
  }
  srsDir_PopCWD(NULL);
  return content;
}
