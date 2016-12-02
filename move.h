#ifndef MOVE_H_
#define MOVE_H_

#include "tile.h"
#include "slot.h"

/**@file*/
/** Struct representing a move that a player wishes to make. */
/** @see tile, slot */
struct move {
	struct tile tile; /**< Unique tile descriptor */
	struct slot slot; /**< Where tile is being placed */
	int rotation; /**< Rotation to apply to tile */
	int tcorner; /**< where the tiger is if it's there. */
	int ccorner;  /**< Where the crocodile is if it's there. */
};

/** Error code enumeration. */
enum game_error_code {
	OK = 0,
	E_TILE_NOT_PLACEABLE = 1,
	E_TILE_EDGE_CONFLICT = 2
};

/**	Creates and returns move struct.
*	@param t The tile to be placed.
*	@param s The spot in which the tile will be placed.
* @param rotation The rotation state of the tile.
*	@param tcorner The location of a tiger to be placed on the tile.
* @param ccorner The location of a crocodile to be placed on the tile.
* @return The created move struct.
*/
struct move make_move(struct tile t, struct slot s, int rotation,
		int tcorner, int ccorner);

#endif
