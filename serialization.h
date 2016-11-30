#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <stdint.h> /* uint8_t */
#include <unistd.h> /* size_t */

#include "edge.h"
#include "tile.h"
#include "move.h"
#include "limits.h"

#define TILE_SZ 6		/* 1 byte per edge + ceneter + attribute */
#if 0
#define MOVE_SZ TILE_SZ + 3	/* +1 for x, y, rotation */
#endif
#define MOVE_SZ TILE_SZ + 5	/* +1 for x, y, rotation, tiger & crocodile */

uint8_t *serialize_tile(struct tile t, uint8_t *buf);
struct tile deserialize_tile(uint8_t buf[TILE_SZ]);
uint8_t *serialize_move(struct move m, uint8_t *buf);
struct move deserialize_move(uint8_t buf[MOVE_SZ]);
void print_buffer(uint8_t *buf, size_t len);

#endif
