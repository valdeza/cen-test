#include "serialization.h"

uint8_t *serialize_tile(struct tile t, uint8_t *buf)
{
	for (size_t i = 0; i < 5; ++i) {
		buf[i] = t.edges[i];
	}
	buf[5] = t.attribute;
	return &buf[6];
}

struct tile deserialize_tile(uint8_t buf[TILE_SZ])
{
	enum edge edges[5];
	for (size_t i = 0; i < 5; ++i) {
		edges[i] = buf[i];
	}
	enum attribute a = buf[5];
	return make_tile(edges, a);
}

uint8_t *serialize_move(struct move m, uint8_t *buf)
{
	buf = serialize_tile(m.tile, buf);
	uint8_t x = m.slot.x, y = m.slot.y;
	uint8_t rotation = m.rotation;
	*buf++ = x;
	*buf++ = y;
	*buf++ = rotation;
	return buf;
}

struct move deserialize_move(uint8_t buf[MOVE_SZ])
{
	struct tile t = deserialize_tile(buf);
	return make_move(t, make_slot(buf[6], buf[7]), buf[8], -1, -1);
}

void print_buffer(uint8_t *buf, size_t len)
{
	for (size_t i = 0; i < len; ++i) {
		printf("%.2d ", buf[i]);
	}
	printf("\n");
}

