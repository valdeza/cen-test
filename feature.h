#ifndef FEATURE_H_
#define FEATURE_H_

#include <assert.h> /* assert() */
#include <unistd.h> /* size_t */

#include "limits.h" /* PLAYER_COUNT */
#include "edge.h" /* enum edge */
#include "slot.h" /* struct slot, compare_slots() */
#include "tile.h" /* struct tile */
#include "move.h" /* struct move */

/**@file*/
/** Representation of any feature (e.g. lake, jungle, or gametrail)
*/
struct feature {
	size_t weighted_size; /**< The number of tiles the feature crosses, weighted for different score affectors (i.e. animals). */
	size_t tigers[PLAYER_COUNT]; /**< Array representing the number of tigers each player has on the feature. */
	size_t crocodiles[PLAYER_COUNT]; /**< Array representing the number of crocodiles each player has on the feature. */
	size_t neighbors[100]; /**< Array of indices into the features array on the game object. Shows what features are adjacent to this one.*/
	size_t neighbor_count; /**< The number of features that are adjacent to this feature. */
	struct slot open_slots[100]; /**< Array of open slots around the feature.*/
	size_t slot_count; /**< Number of open slots around the feature. */
	enum edge type; /**< Type of feature (e.g. lake, jungle, gametrail). TODO: Den */
};

/**	Link feature to adjacent feature appropriately upon playing a move.
* @param m Move played.
*	@param neighbors Pointer to array of adjacent features.
*	@param f Array of all features existing in the game.
* @param features_used Number of features existing in the game.
*	@return 0 if the function successfully ran, 1 if there was an allocation failure.
*/
int play_move_feature(struct move m, struct slot **neighbors,
		struct feature **f, size_t *features_used);

/**	Ensure that tiger placement is valid.
*	@param m The current move being made.
*	@param player The player making the move. 0 = player 1, 1 = player 2
*	@param f Array of all features existing in the game.
*	@return 0 if the tiger placement is valid. 1 if the placement is invalid.
*/
int test_meeple(struct move m, int player, struct feature **f);

/**	Place tiger for current move.
*	@param m The current move being made.
*	@param player The player making the move. 0 = player 1, 1 = player 2
*	@param f Array of all features existing in the game.
*	@return 0 if the tiger was successfully placed. 1 if the placement was invalid.
*/
int play_meeple(struct move m, int player, struct feature **f);

/** Update the scores of each player.
*	@param scores Array with player scores.
*	@param scratch Array for holding the list of features
*	@param a Array of all features in game mapped to board coordinates.
*	@param alen Length of array <em>a</em>.
*/
void update_scores(size_t *scores, struct feature **scratch,
		struct feature **a, size_t *alen);

#endif
