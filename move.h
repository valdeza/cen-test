#ifndef MOVE_H_
#define MOVE_H_

#include "tile.h"
#include "slot.h"

struct move {
	struct tile tile; /** Unique tile descriptor */
	struct slot slot; /** Where tile is being placed */
	int rotation; /** Rotation to apply to tile */
};

enum game_error_code {
	OK = 0,
	E_TILE_NOT_PLACEABLE = 1,
	E_TILE_EDGE_CONFLICT = 2
};

struct move make_move(struct tile t, struct slot s, int rotation);

#endif
