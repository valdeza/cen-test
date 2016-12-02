#ifndef GAME_H_
#define GAME_H_

/* clock_gettime() */
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */
#include <sys/time.h>
#include <time.h>

#include <stddef.h>	/* size_t */
#include <math.h>	/* round() */
#include <assert.h>	/* assert() */
#include <pthread.h>

#include "limits.h"
#include "tile.h"
#include "board.h"
#include "feature.h"
#include "rngs/mt19937-64.h" /* Mersenne Twister PRNG. Try PCG if too slow */

#define MEEPLE_COUNT 7
#define PLAYER_COUNT 2

/**@file*/
/** Holds all information on the game state.
*	@see board, feature
*/
struct game {
	struct board board; /**< The board on which the game is played and tiles are placed.*/
	struct tile tile_deck[TILE_COUNT]; /**< All tiles to be played in the game. */
	size_t tiles_used; /**< The number of tiles used during the game. */
	size_t scores[PLAYER_COUNT]; /**< Holds the players' scores. */
	size_t meeples[PLAYER_COUNT]; /**< The number of tigers each player holds. */
	struct feature *features[AXIS * AXIS * 4 * 3]; /**< Mapping of each space on the board to the feature occupying that space. */
	size_t features_used; /**< The number of features in the game. */
};

/**	Calculate and update the scores of each player.
*	@param g The game state struct.
*/
void calculate_scores(struct game *g);

/**	Create the game.
*	@param g The game state struct.
*/
void make_game(struct game *g);

/**	Free up space taken by game.
*	@param g The game state struct.
*/
void free_game(struct game *g);

/** Writes the given deck into the given game's tile_deck by deep copy.
* @param g The game struct.
*	@param deck The tile deck array to be written.
*/
void set_game_deck(struct game *g, struct tile *deck);

/**	Play the current move for the currently active player.
* @param g The game struct.
*	@param m The move to be played.
*	@param player The player making the move.
*	@return 0 if the move was able to be played, game_error_code error code if the move is invalid.
*/
int play_move(struct game *g, struct move m, int player);

/** Check if the current move is valid.
* @param g The game struct.
*	@param m The move to be played.
*	@param player The player making the move.
*	@return 0 if the move is valid, game_error_code error code if the move is invalid.
*/
int is_move_valid(struct game *g, struct move m, int player);

/**	Check if the <em>tile_deck</em> is empty.
*	@param g The game struct
*	@return TRUE if the <em>tile_deck</em> is empty, FALSE if it is not.
*/
bool is_tile_deck_empty(struct game *g);

/**	Draw a tile from the <em>tile_deck</em>.
*	@param g The game struct.
*	@return The drawn tile.
*/
struct tile deal_tile(struct game *g);

/**	For the given tile, find all possible moves given the game's current state.
*	@param g The game struct.
*	@param player The currently active player.
*	@param t The tile to be played.
*	@param pmoves Array to be filled with all possible moves.
*	@param pmoves_len The maximum number of moves that can be made.
*/
void generate_available_moves(struct game *g, int player, \
		struct tile t, struct move *pmoves, size_t *pmoves_len);
#endif
