#ifndef SLOT_H_
#define SLOT_H_

/** Represents x,y coordinates. */
struct slot {
	unsigned int x;
	unsigned int y;
};

struct slot make_slot(unsigned int x, unsigned int y);
int compare_slot_positions(struct slot a, struct slot b);
struct slot* get_adjacent_slots(struct slot* adj, struct slot s);

#endif
