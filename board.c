#include "board.h"

static size_t index_slot(struct slot s)
{
	return AXIS * s.x + s.y;
}

static int slot_on_board(struct slot s)
{
	if (s.x < AXIS && s.y < AXIS) {
		return 1;
	}
	return 0;
}

static void list_adjacent_slots(struct slot s, struct slot **adjs)
{
	/*			  up	 right	 bottom	   left */
	int n[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
	/* Check adjacent tiles to make sure edges match. */
	for (int i = 0; i < 4; ++i) {
		struct slot sprime = make_slot(s.x + n[i][0], s.y + n[i][1]);
		if (!slot_on_board(s)) {
			adjs[i] = NULL;
		} else {
			*(adjs[i]) = sprime;
		}
	}
}

static int slot_placeable(struct board b, struct slot s)
{
	/* TODO: Switch to linear search? */
	/* Linear search open positions for the desired one. */
	for (unsigned i = 0; i < b.sps; ++i) {
		switch(compare_slots(b.slot_spots[i], s)) {
		case -1:
			continue;
		case 0:
			return i + 1;
		case 1:
			return 0;
		}
	}
	return 0;
}

static int slot_empty(struct board b, struct slot s)
{
	struct tile t = b.tiles[index_slot(s)];
	for (int i = 0; i < 5; ++i) {
		if (t.edges[i] != EMPTY) {
			return 0;
		}
	}
	return 1;
}

size_t find_spot(struct slot *slots, size_t count, struct slot s)
{
	size_t i;
	for (i = 0; i < count && compare_slots(s, slots[i]) > 0; ++i) {}
	return i;
}

static struct board add_placeable_slot(struct board b, struct slot s)
{
	struct slot *spots = b.slot_spots;
	size_t i = find_spot(spots, b.sps, s);
	if (i < b.sps) { /* Make room for the element (Sorted insert). */
		memmove(&b.slot_spots[i + 1], &spots[i], sizeof(s) * b.sps - i);
	}
	spots[i] = s;
	b.sps++;
	return b;
}

static struct board remove_placeable_slot(struct board b, struct slot s)
{
	struct slot *spots = b.slot_spots;
	size_t i = find_spot(spots, b.sps, s);
	memmove(&spots[i], &spots[i + 1], sizeof(s) * (b.sps-- - i));
	return b;
}

static struct board update_slot_spots(struct board b, struct slot s)
{
	struct slot adj[4];
	struct slot *adjs[4];
	for (size_t i = 0; i < 4; ++i) {
		adjs[i] = &adj[i];
	}
	list_adjacent_slots(s, adjs);
	b = remove_placeable_slot(b, s);
	for (int i = 0; i < 4; ++i) {
		if (adjs[i] == NULL) {
			continue;
		}
		if (slot_empty(b, *adjs[i])) {
			b = add_placeable_slot(b, adj[i]);
		}
	}
	return b;
}

/* TODO: Switch int error codes to error enums for cleanliness. */
static int invalid_move(struct board b, struct move m, struct slot **adjs)
{
	if (!slot_placeable(b, m.slot)) {
		return 1; /* Slot not placeable. */
	}
	list_adjacent_slots(m.slot, adjs);
	for (unsigned int i = 0; i < 4; ++i) { /* Need wrapping */
		if (adjs[i] == NULL) { /* Ignore if not on board. */
			continue;
		}
		/* The (i + 2) % 4 math here is a bit evil, but it works. */
		enum edge pair = b.tiles[index_slot(*adjs[i])].edges[(i+2)%4];
		if (pair == EMPTY) {
			continue; /* Empty tiles match with everything. */
		}
		if (pair != m.tile.edges[i]) { /* Corresponding don't match. */
			return 2;
		}
	}
	return 0;
}

struct board make_board(void)
{
       struct board b;
       enum edge edges[5] = { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY };
       const unsigned int mid = (AXIS - 1) / 2; /* Must start in center. */
       b.slot_spots[0] = make_slot(mid, mid);
       b.sps = 1;
       for (unsigned int i = 0; i < AXIS*AXIS; ++i) {
		b.tiles[i] = make_tile(edges, NONE);
       }
       /* Tab between columns except for the last one, which newlines. */
       memset(b.column_terminators, '\t', AXIS - 1);
       b.column_terminators[AXIS - 1] = '\n';
       return b;
}

char *print_board(struct board b, char res[BOARD_LEN])
{
	const size_t cnt = TILE_LINES;
	const size_t len = TILE_LINE_LEN;
	char buf[TILE_LEN];

	/* Pretty print the board in NxN format. */
	for (size_t i = 0; i < AXIS; ++i) {
		for (size_t j = 0; j < AXIS; ++j) {
			print_tile(b.tiles[index_slot(make_slot(i, j))], buf);
			for (size_t k = 0; k < cnt; ++k) {
				const size_t ind = ((i *cnt +k) *AXIS +j) *len;
				buf[(k + 1) *len - 1] = b.column_terminators[j];
				memcpy(&res[ind], &buf[len * k], len);
			}
		}
	}
	res[BOARD_LEN - 1] = '\0';
	return res;
}

/* See TODO for invalid_move. */
int play_move_board(struct board *b, struct move m, struct slot **adj)
{
	int rc;
	if ((rc = invalid_move(*b, m, adj))) {
		return rc;
	}
	b->tiles[index_slot(m.slot)] = rotate_tile(m.tile, m.rotation);
	*b = update_slot_spots(*b, m.slot);
	return 0;
}

#ifdef TEST
static void print_placeable_slots(struct board b)
{
	printf("Slots:\n");
	printf("X\tY\n");
	for (size_t i = 0; i < b.sps; ++i) {
		printf("%u\t%u\n", b.slot_spots[i].x, b.slot_spots[i].y);
	}
	return;
}

static void play_and_check_move(struct board *b,struct move m,struct slot **adj)
{
	int rc;
	if ((rc = play_move_board(b, m, adj))) {
		printf("Invalid move! %d\n", rc);
	} else {
		printf("Good move!\n");
	}
}

int main(void)
{
	char buffer[TILE_LEN];
	char board_buffer[BOARD_LEN];
	enum edge edges[5][5] = {
		{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
		{ ROAD, ROAD, ROAD, ROAD, ROAD },
		{ FIELD, FIELD, FIELD, FIELD, FIELD },
		{ CITY, CITY, CITY, CITY, CITY },
		{ CITY, FIELD, ROAD, CITY, ROAD }
	};
	struct tile tiles[5] = {
		make_tile(edges[0], NONE),
		make_tile(edges[1], NONE),
		make_tile(edges[2], NONE),
		make_tile(edges[3], NONE),
		make_tile(edges[4], NONE)
	};

	const char string[5][30] = {
		"\nEmpty tile:",
		"\nAll Road tile:",
		"\nAll Field tile:",
		"\nAll City tile:",
		"\nMixed tile:"
	};

	printf("Testing different tile types.\n");
	for (int i = 0; i < 5; ++i) {
		printf("%s\n%s\n", string[i], print_tile(tiles[i], buffer));
	}

	printf("\nTile Rotations: \n");
	for (int i = 0; i < 4; ++i) {
		printf("%d rotation:\n%s\n", i,
			print_tile(rotate_tile(tiles[4], i), buffer));
	}

	printf("\nTesting board creation. All Null.\n");
	struct board b = make_board();
	printf("%s\n", print_board(b, board_buffer));

	printf("\nLet's see what slots are placeable.\n");
	print_placeable_slots(b);
	const unsigned int mid = AXIS / 2;
	printf("\nPlay the center (%d, %d), the starting move.\n", mid, mid);
	struct slot adj[4];
	struct slot *adjs[4];
	for (size_t i = 0; i < 4; ++i) {
		adjs[i] = &adj[i];
	}
	play_and_check_move(&b,
			make_move(tiles[3], make_slot(mid, mid), 0), adjs);
	printf("%s\n", print_board(b, board_buffer));

	printf("\nAnd now what slots are placeable?\n");
	print_placeable_slots(b);

	printf("\nTest tile validator (should fail): (%d, %d)\n", mid, mid + 1);
	for (size_t i = 0; i < 4; ++i) {
		adjs[i] = &adj[i];
	}
	play_and_check_move(&b,make_move(tiles[2],
				make_slot(mid, mid + 1), 0), adjs);
	printf("%s\n", print_board(b, board_buffer));
	print_placeable_slots(b);

	printf("\nTest tile validator (should pass): (%d, %d)\n", mid, mid + 1);
	for (size_t i = 0; i < 4; ++i) {
		adjs[i] = &adj[i];
	}
	play_and_check_move(&b,
			make_move(tiles[3], make_slot(mid, mid + 1), 0), adjs);
	printf("%s\n", print_board(b, board_buffer));
	print_placeable_slots(b);

	return 0;
}
#endif
