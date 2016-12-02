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

/**@file*/
/**	Convert tile struct into byte array for network transfer
*	@param t The tile to be serialized.
*	@param buf The buffer which holds the serialized tile.
*	@return A reference to the buffer containing the serialized tile.
*/
uint8_t *serialize_tile(struct tile t, uint8_t *buf);

/**	Convert serialized tile received over network transfer into regular tile struct.
*	@param buf The buffer which holds the serialized tile.
*	@return tile struct.
*/
struct tile deserialize_tile(uint8_t buf[TILE_SZ]);

/**	Convert move struct into byte array for network transfer.
*	@param m The move to be serialized.
*	@param buf The buffer which holds the serialized move.
*	@return A reference to the buffer containing the serialized tile.
*/
uint8_t *serialize_move(struct move m, uint8_t *buf);

/**	Convert serialized move received over network transfer into regular move struct.
*	@param buf The buffer which holds the serialized move.
*	@return move struct.
*/
struct move deserialize_move(uint8_t buf[MOVE_SZ]);

/**	Print a buffer containing a byte array used for network transfer
*	@param buf The buffer to be printed.
*	@param len The length of the buffer.
*/
void print_buffer(uint8_t *buf, size_t len);

#endif
