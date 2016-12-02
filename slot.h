#ifndef SLOT_H_
#define SLOT_H_

/**@file*/
/** Represents x,y coordinates. */
struct slot {
	unsigned int x;/**< The x coordinate. */
	unsigned int y;/**< The y coordinate. */
};

/**	Make a slot struct.
*	@param x The x coordinate
*	@param y The y coordinate.
*	@return The created slot struct.
*/
struct slot make_slot(unsigned int x, unsigned int y);

/**	Compare the positions (x,y coordinates) of two, adjacent slots.
*	@param a First slot.
*	@param b Second slot.
*	@return negative value if <em>a</em> is to the left of or above <em>b</em>, 0 if <em>a</em> and <em>b</em> are the same slot, or a positive value if <em>a</em> is to the right of or above <em>b</em>.
*/
int compare_slot_positions(struct slot a, struct slot b);

/**	Get all slots adjacent to the input slot.
*	@param adj slot pointer to hold the adjacent slots.
*	@param s The slot for which the the adjacent slots are found.
*	@return An array of the 4 slots adjacent to the input slot. 0 for above, 1 for right, 2 for below, 3 for left.
*/
struct slot* get_adjacent_slots(struct slot* adj, struct slot s);

#endif
