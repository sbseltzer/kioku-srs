/**
 * @addtogroup REST
 *
 * Module for RESTful server implementation.
 *
 * @{
 */
#ifndef _KIOKU_REST_H
#define _KIOKU_REST_H

#define KIOKU_REST_API_VERSION "v1"
#define KIOKU_REST_API_PATH "/api/" KIOKU_REST_API_VERSION "/"

/** REST API URIs
 * /decks/<username> - data[deck-index] = deck-id
 * /notes/<deck-id> - data[note-id] = note
 * /cards/<note-id> - data[card-id] = card
 */

kiokuAPI char *srsREST_GetNextCard(const char *deck_id);
kiokuAPI bool  srsREST_GradeCard(const char *card_id, int grade);

#endif /* _KIOKU_REST_H */

/** @} */
