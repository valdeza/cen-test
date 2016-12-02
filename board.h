#ifndef BOARD_H_
#define BOARD_H_

#include <stdio.h>
#include <string.h>	/* memcpy */
#include <stdlib.h>	/* free() */
#include <stdbool.h>
#include "edge.h"	/* edges. */
#include "tile.h"	/* tiles. */
#include "slot.h"	/* slots. */
#include "move.h"	/* moves. */
#include "limits.h"	/* sizes of things. */

#define BOARD_LEN AXIS * AXIS * (TILE_LEN - 1) + 1

/**@file*/
/**	Represents the playing board.
*	@see tile, slot
*/
struct board {
	struct tile tiles[AXIS*AXIS]; /**< Array for holding all the tiles placed on the board. */
	struct slot slot_spots[AXIS*AXIS]; /**< Array for holding all the slots on the board. */
	unsigned int empty_slot_count; /**< The number of available slots on the board. */
	char column_terminators[AXIS]; /**< Represents the end of each column, for board printing. */
};

/**	Creates a board struct.
*	@return The instantiated board.
*/
struct board make_board(void);

/**	Fill a character array with the character representation of the board.
* @param b The board to be printed.
*	@param res The character array to be filled.
*	@return Pointer to the character array representing the board.
*/
char *print_board(struct board b, char res[BOARD_LEN]);

/**	Check the input move, and play if valid.
*	@see move, slot, game_error_code
*	@param b Pointer to the board on which the move is played.
*	@param m The move to be played.
*	@param adjs Pointer to array of slots adjacent to the slot where the move is taking place.
* @return Error code indicating whether or not the move was valid. 0 if valid, non-zero if not valid.
*/
enum game_error_code \
	play_move_board(struct board *b, struct move m, struct slot **adjs);

/**	Check the input move for validity.
*	@see move, slot, game_error_code
*	@param b Pointer to the board on which the move is played.
*	@param m The move to be played.
*	@param adjs Pointer to array of slots adjacent to the slot where the move is taking place.
* @return Error code indicating whether or not the move was valid. 0 if valid, non-zero if not valid.
*/
enum game_error_code \
	test_move_board(struct board *b, struct move m, struct slot **adjs);
#endif
