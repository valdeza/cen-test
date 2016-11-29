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

static void add_to_group(int *adj, unsigned int leader,
		unsigned int lead_offset, unsigned int follower)
{
	adj[leader * 12 + lead_offset] = follower + 1;
	adj[follower * 12] = 0;
	adj[follower * 12 + 1] = leader;
}

static void connect_road(int adj[12*12], unsigned int side, struct tile t)
{
	/* left looks anticlockwise, mid & right look clockwise. */
	unsigned int r[3][4] ={ {3, 0, 1, 2}, {1, 2, 3, 0}, {1, 2, 3, 0} };
	/* Variables are unsigned because we need wrapping. */
	for (unsigned int third = 0; third < 3; ++third) {/* left, mid, right */
		unsigned int offset = 1;
		if (adj[(side * 3 + third) * 12] == 0) {
			continue;
		}
		for (unsigned int k=r[third][side]; k!=side; k=r[third][k]) {
			if (t.edges[k] != t.edges[side]) {
				continue;
			}
			add_to_group(adj,
				side * 3 + third, offset++, k * 3 + 2 - third);
			/* Only the middle thirds are multi-attached */
			if (third != 1) {
				break;
			}
		}
	}
}

static void connect_field_city(int adj[12*12], unsigned int i, struct tile t)
{
	const enum edge edge = t.edges[i];
	const enum edge center_edge = t.edges[4];
	unsigned int offset = 1;

	for (unsigned int k = 1; k < 3; ++k) {
		/* All of this edge's other corners are in our group. */
		add_to_group(adj, i * 3, offset++, i * 3 + k);
	}
	if (edge == CITY && center_edge != CITY) {
		return; /* Cities are only connected by center city */
	}
	for (unsigned int j = i + 1; j < 4; ++j) {
		if (t.edges[j] != edge) { /* Dissimilar sides not connected. */
			continue;
		}
		for (unsigned int k = 0; k < 3; ++k) { /* Add side to group */
			add_to_group(adj, i * 3, offset++, j * 3 + k);
		}
	}
	if (edge == CITY) {
		return;
	}
	/* 
	 * Additionally, fields can be connected to the roads to their
	 * immedieate clockwise and anticlockwise.
	*/
	unsigned int j = (i + 1) % 4; /* Clockwise */
	if (t.edges[j] == ROAD) {
		add_to_group(adj, i * 3, offset++, j * 3);
	}

	j = (i - 1) % 4; /* Anticlockwise */
	if (t.edges[j] == ROAD) {
		add_to_group(adj, i * 3, offset++, 3 * (j + 1) - 1);
	}
}

void init_adj(const struct tile t, int adj[12*12])
{
	memset(adj, 0, sizeof(int) * 12 * 12);
	for (unsigned int i = 0; i < 12; ++i) {
		adj[i * 12] = i + 1; /* nonzero value (assume leader). */
	}
	/* TODO: Refactor into loops elements into functions. */
	/* NOTE: The algorithm assumes fields and cities are connected
	 * before roads, in order to handle the case where a field shares
	 * a connection (a side of) a road.
	*/
	const enum edge center_edge = t.edges[4];
	for (unsigned int i = 0; i < 4; ++i) { /* Connect roads and fields */
		const enum edge edge = t.edges[i];
		if (adj[(i * 3) * 12] == 0) { /* Already in a group. */
			continue;
		}
		if (edge != CITY && edge != FIELD) {
			continue;
		}
		connect_field_city(adj, i, t);
	}
	for (unsigned int i = 0; i < 4; ++i) { /* connect roads. */
		const enum edge edge = t.edges[i];
		if (edge != ROAD) {
			continue;
		}
		connect_road(adj, i, t);
	}
}
