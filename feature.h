#ifndef FEATURE_H_
#define FEATURE_H_

#include <assert.h> /* assert() */
#include <unistd.h> /* size_t */

#include "limits.h" /* PLAYER_COUNT */
#include "edge.h" /* enum edge */
#include "slot.h" /* struct slot, compare_slots() */
#include "tile.h" /* struct tile */
#include "move.h" /* struct move */

struct feature {
	size_t weighted_size;
	size_t meeples[PLAYER_COUNT]; // The number of meeples for each player.
	size_t neighbors[100]; /*
		An array of indices into the graphs array on the game object.
		Shows what features are adjacent to this one.
	*/
	size_t neighbor_count;
	struct slot open_slots[100];	/* Open slots for the feature. */
	size_t slot_count;		/* Number of open slots. */
	enum edge type; /* Road, City, Field, TODO: Monestary */
};

int play_move_feature(struct move m, struct slot **neighbors,
		struct feature **f, size_t *features_used);

int play_meeple(struct move m, int player, int cnr, struct feature **f);
void update_scores(size_t *scores, struct feature **scratch,
		struct feature **a, size_t *alen);

#endif
