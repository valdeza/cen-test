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

struct board {
	struct tile tiles[AXIS*AXIS];
	struct slot slot_spots[AXIS*AXIS];
	unsigned int empty_slot_count;
	char column_terminators[AXIS];
};

size_t get_index_from_slot(struct slot s);
struct board make_board(void);
void list_adjacent_slots(struct slot s, struct slot **adjs);
void list_adjacent_tiles(struct board *b, struct slot **ss, struct tile **ts);
char *print_board(struct board b, char res[BOARD_LEN]);
enum game_error_code play_move_board(struct board *b, struct move m);
enum game_error_code test_move_board(struct board *b, struct move m);
#endif
