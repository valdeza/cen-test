#include "slot.h"

struct slot make_slot(unsigned int x, unsigned int y)
{
	struct slot s = {
		.x = x,
		.y = y
	};
	return s;
}

int compare_slots(struct slot a, struct slot b)
{
	if (a.x == b.x) {
		return a.y - b.y;
	}
	return a.x - b.x;
}
