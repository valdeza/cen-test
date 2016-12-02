#include "slot.h"

struct slot make_slot(unsigned int x, unsigned int y)
{
	struct slot s = {
		.x = x,
		.y = y
	};
	return s;
}

/** Returns the relative position of <em>a</em> to <em>b</em>;
 * x position takes precedence over y position.
 * 	If <em>a</em> comes before <em>b</em>, -1 is returned.
 * 	If <em>a</em> is the same position as <em>b</em>, 0 is returned.
 * 	If <em>a</em> comes after <em>b</em>, 1 is returned.
 */
int compare_slot_positions(struct slot a, struct slot b)
{
	if (a.x == b.x) {
		return a.y - b.y;
	}
	return a.x - b.x;
}

/** Fetches the slots adjacent to the given slot and
 * stores them into <em>adj</em> in the order of top, right, bottom, left.
 *
 * @returns <em>adj</em>
 * @precondition <em>adj</em> is an array of at least length 4 to store all adjacent slots.
 */
struct slot* get_adjacent_slots(struct slot* adj, struct slot s)
{
	adj[0] = make_slot(s.x  , s.y+1); /* top */
	adj[1] = make_slot(s.x+1, s.y  ); /* right */
	adj[2] = make_slot(s.x  , s.y-1); /* bottom */
	adj[3] = make_slot(s.x-1, s.y  ); /* left */

	return adj;
}
