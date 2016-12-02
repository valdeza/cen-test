#ifndef TILE_H_
#define TILE_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>	/* malloc() */
#include <string.h>	/* memcpy() */
#include "edge.h"	/* edges. */

#define TILE_LINE_LEN 4
#define TILE_LINES 3
#define TILE_LEN TILE_LINE_LEN * TILE_LINES + 1 /* Null terminator */

/**@file*/
/**	Enumeration representing the non-feature tile attributes. */
enum attribute {
	NONE = 0, /**< No attribute */
	PREY = 1, /**< A prey animal */
	DEN = 2, /**< A den */
};

/** Represents a tile
*/
struct tile {
	enum edge edges[5]; /* 0 = Top, 1 = Right, 2 = Bottom, 3 = Left, 4 = Center. */ /**< Stores the types of features on each tile.*/
	enum attribute attribute; /**< Stores the attribute on the tile. */
};

/**	Compare two tiles, and see if they are the same type.
*	@param a First tile.
* @param b Second tile.
*	@return True the tiles are the same, and false if they are different.
*/
bool is_tile_equal(struct tile a, struct tile b);

/**	Create a tile struct.
*	@param edges The features on the tile.
*	@param a The attribute on the tile.
*	@return A tile struct.
*/
struct tile make_tile(const enum edge edges[5], enum attribute a);

/** Rotate a tile clockwise by a certain number of 90 degree increments, swapping entries in the edges array as needed.
*	@param old The tile to be rotated.
*	@param rotation The number of 90 degree clockwise rotations to perform
*	@return Rotated tile struct.
*/
struct tile rotate_tile(const struct tile old, const int rotation);

/**	Fill an array with the character representation of a tile.
*	@param t The tile to be printed.
*	@param b Array to hold the character representation of a tile.
* @return Array holding the character representation of a tile.
*/
char *print_tile(struct tile t, char b[TILE_LEN]);

/**	Initialize the tile's adjacency matrix.
*	@param t Tile for which the adjacency matrix will be created.
*	@param adj The adjaceny matrix.
*/
void init_adj(const struct tile t, int adj[12*12]);

#endif
