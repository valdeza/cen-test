#include "tile.h"

bool is_tile_equal(struct tile a, struct tile b)
{
	for (int i = 0; i < 5; ++i) {
		if (a.edges[i] != b.edges[i]) {
			return false;
		}
	}
	if (a.attribute != b.attribute) {
		return false;
	} else {
		return true;
	}
}

struct tile make_tile(const enum edge edges[5], enum attribute a)
{
	struct tile t;
	t.attribute = a;
	memcpy(&t.edges, edges, sizeof(edges[0]) * 5);
	return t;
}

/**
 * @param old Tile to rotate
 * @param int Number of clockwise 90 degree rotations to perform
 * @returns Rotated tile
 */
struct tile rotate_tile(const struct tile old, const int rotation)
{
	enum edge new[5];
	new[4] = old.edges[4]; /* Center doesn't move. */
	
	/* Clockwise rotation moves previous elements up, so we go backwards. */
	for (unsigned int i = 4; i > 0; --i) { /* Need unsigned for wrapping */
		new[i - 1] = old.edges[(i - 1 - rotation) % 4];
	}
	return make_tile(new, old.attribute);
}

char *print_tile(const struct tile t, char b[TILE_LEN])
{
	/* Our array stores in clockwise order starting at the top.
	 * So the tile looks like this: (indexes)
	 * + 0 + |  0  1  2  3
	 * 3 4 1 |  4  5  6  7
	 * + 2 + |  8  9 10 11
	*/
	const char c[4] = {'X', 'C', 'F', 'R'};

	/* Make borders and null terminate. */
	for (int i = 1; i <= 3; i++) {
		b[i * 4 - 1] = '\n';
	}
	for (int i = 0; i < 6; i++) {
		b[i * 2] = '+';
	}
	b[12] = '\0';

	/* Place actual characters. */
				b[1] = c[t.edges[0]];
	b[4] = c[t.edges[3]];	b[5] = c[t.edges[4]]; b[6] = c[t.edges[1]];
				b[9] = c[t.edges[2]];
	return b;
}
