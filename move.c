#include "move.h"

struct move make_move(struct tile t, struct slot s, int rotation,
		int tcorner, int ccorner)
{
	struct move m = {
		.tile = t,
		.slot = s,
		.rotation = rotation,
		.tcorner = tcorner,
		.ccorner = ccorner
	};
	return m;
}

