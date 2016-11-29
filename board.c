#include "board.h"

/** Gets the array index from the provided slot.
 * The slot is recorded with x and y values for graphic representation.
 * The array used for storing all slots is an AXIS*AXIS length array.
 */
static inline size_t get_index_from_slot(struct slot s)
{
	return AXIS * s.x + s.y;
}

/** Returns whether the given slot's x/y position
 * exists somewhere within (0, 0) to (AXIS, AXIS)
 */
static inline bool is_slot_in_boundary(struct slot s)
{
	return (s.x < AXIS && s.y < AXIS);
}

static void list_adjacent_slots(struct slot s, struct slot **adjs)
{
	/*			  up	 right	 bottom	   left */
	int n[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
	/* Check adjacent tiles to make sure edges match. */
	for (int i = 0; i < 4; ++i) {
		struct slot sprime = make_slot(s.x + n[i][0], s.y + n[i][1]);
		if (is_slot_in_boundary(s)) {
			*(adjs[i]) = sprime;
		} else {
			adjs[i] = NULL;
		}
	}
}

/**
 * @remarks Although slots and tiles have the same length arrays,
 * we search through the board's slot array due to no sorting guarantees.
 */
static bool is_slot_placeable(struct board b, struct slot s)
{
	/* TODO: Switch to linear search? */
	/* Linear search open positions for the desired one. */
	for (unsigned i = 0; i < b.empty_slot_count; ++i) {
		switch(compare_slot_positions(b.slot_spots[i], s)) {
		case -1:
			continue;
		case 0:
			return true;
		case 1:
			return false;
		}
	}
	return false;
}

/** Returns whether the given slot on the given board has no tile on it.
 * @remarks todo: Confirm whether "placing" a tile is done by writing the tile's edge[] onto an empty tile's edge[].
 */
static bool is_slot_empty(struct board b, struct slot s)
{
	struct tile t = b.tiles[get_index_from_slot(s)];
	for (int i = 0; i < 5; ++i) {
		if (t.edges[i] != EMPTY) {
			return false;
		}
	}
	return true;
}

/** Returns the index of <em>slots</em> at/immediately following the
 * given slot <em>s</em>, or <em>count</em> if not found.
 *
 * @precondition <em>slots</em> is sorted by ascending position.
 */
size_t get_insertion_index(struct slot *slots, size_t count, struct slot s)
{
	/* Iterate i until count is reached
	 * or we pass the given slot's x/y position
	 */
	size_t i;
	for (i = 0; i< count && compare_slot_positions(s, slots[i]) > 0; ++i) {}
	return i;
}

/** Inserts the given slot into the given board's slot_spots. */
static struct board add_placeable_slot(struct board b, struct slot s)
{
	struct slot *spots = b.slot_spots;
	size_t i = get_insertion_index(spots, b.empty_slot_count, s);
	if (i < b.empty_slot_count) { /* Make room for the element (Sorted insert). */
		memmove(&spots[i+1], &spots[i], sizeof(s)*b.empty_slot_count-i);
	}
	spots[i] = s;
	b.empty_slot_count++;
	return b;
}

/** Removes the given slot from the given board's slot_spots. */
static struct board remove_placeable_slot(struct board b, struct slot s)
{
	struct slot *spots = b.slot_spots;
	size_t i = get_insertion_index(spots, b.empty_slot_count, s);
	memmove(&spots[i], &spots[i + 1], sizeof(s) * (b.empty_slot_count - i));
	b.empty_slot_count--;
	return b;
}

/** Removes the given slot from the given board's slot_spots
 * and add the newly available slots adjacent to the removed slot.
 *
 * Intended to be called when a tile has been placed
 * in the position of the removed slot.
 */
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
		if (is_slot_empty(b, *adjs[i])) {
			b = add_placeable_slot(b, adj[i]);
		}
	}
	return b;
}

/** Returns the validation code of the given move on the given board.
 * @returns 0 (OK) if a legal valid move, non-zero otherwise.
 * @see move.h:enum game_error_code
 */
static enum game_error_code invalid_move(struct board b, struct move m,
		struct slot **adjs)
{
	if (!is_slot_placeable(b, m.slot)) {
		return E_TILE_NOT_PLACEABLE; /* Slot not placeable. */
	}
	list_adjacent_slots(m.slot, adjs);
	struct tile t = rotate_tile(m.tile, m.rotation);
	for (unsigned int i = 0; i < 4; ++i) { /* Need wrapping */
		if (adjs[i] == NULL) { /* Ignore if not on board. */
			continue;
		}
		/* The (i + 2) % 4 math here is a bit evil, but it works. */
		enum edge pair =
			b.tiles[get_index_from_slot(*adjs[i])].edges[(i+2)%4];
		if (pair == EMPTY) {
			continue; /* Empty tiles match with everything. */
		}
		if (pair != t.edges[i]) { /* Corresponding don't match. */
			return E_TILE_EDGE_CONFLICT;
		}
	}
	return OK;
}

/** Returns an initialised board with an empty slot in the very centre. */
struct board make_board(void)
{
       struct board b;
       /* Starting centre piece */
       enum edge edges[5] = { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY };
       const unsigned int mid = (AXIS - 1) / 2; /* Must start in center. */
       b.slot_spots[0] = make_slot(mid, mid);
       b.empty_slot_count = 1;
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
			print_tile(b.tiles[get_index_from_slot(make_slot(i, j))], buf);
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

/** Tries to play the given move on the given board, returning a status code.
 *
 * @postcondition Board is updated if given move is valid.
 * @returns 0 (OK) on success, otherwise a respective <code>game_error_code</code>
 */
enum game_error_code
play_move_board(struct board *b, struct move m, struct slot **adjs)
{
	enum game_error_code rc;
	if ((rc = invalid_move(*b, m, adjs))) {
		return rc;
	}
	b->tiles[get_index_from_slot(m.slot)] = rotate_tile(m.tile, m.rotation);
	*b = update_slot_spots(*b, m.slot);
	return OK;
}

#ifdef TEST
static void print_placeable_slots(struct board b)
{
	printf("Slots:\n");
	printf("X\tY\n");
	for (size_t i = 0; i < b.empty_slot_count; ++i) {
		printf("%u\t%u\n", b.slot_spots[i].x, b.slot_spots[i].y);
	}
	return;
}

static void play_and_check_move(struct board *b,struct move m,struct slot **adj)
{
	enum game_error_code rc;
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
			make_move(tiles[3], make_slot(mid, mid), 0, -1, -1),
			adjs);
	printf("%s\n", print_board(b, board_buffer));

	printf("\nAnd now what slots are placeable?\n");
	print_placeable_slots(b);

	printf("\nTest tile validator (should fail): (%d, %d)\n", mid, mid + 1);
	for (size_t i = 0; i < 4; ++i) {
		adjs[i] = &adj[i];
	}
	play_and_check_move(&b,make_move(tiles[2],
			make_slot(mid, mid + 1), 0, -1, -1),
			adjs);
	printf("%s\n", print_board(b, board_buffer));
	print_placeable_slots(b);

	printf("\nTest tile validator (should pass): (%d, %d)\n", mid, mid + 1);
	for (size_t i = 0; i < 4; ++i) {
		adjs[i] = &adj[i];
	}
	play_and_check_move(&b,
			make_move(tiles[3], make_slot(mid, mid + 1), 0, -1, -1),
			adjs);
	printf("%s\n", print_board(b, board_buffer));
	print_placeable_slots(b);

	return 0;
}
#endif
