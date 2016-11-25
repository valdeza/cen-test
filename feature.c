#include "feature.h"

static void init_feature(struct feature *f, enum edge type)
{
	f->weighted_size = f->neighbor_count = f->slot_count = 0;
	for (int i = 0; i < PLAYER_COUNT; ++i) {
		f->meeples[i] = 0;
	}
	f->type = type;
}

static unsigned int count_roads(struct tile t)
{
	int road_count = 0;
	for (int i = 0; i < 4; ++i) {
		if (t.edges[i] == ROAD) {
			road_count++;
		}
	}
	return road_count;
}

/* General solution. Monasterys are different. Matches up connected graphs */
static void init_adj(struct tile t, int *adj)
{
	memset(adj, 0, 12 * 12);
	for (unsigned int i = 0; i < 12; ++i) {
		adj[i * 12] = 1; /* bs nonzero value to init. */
	}
	const unsigned int roadcount = count_roads(t);
	const enum edge center_edge = t.edges[4];
	for (unsigned int i = 0; i < 4; ++i) {
		unsigned int ind = (i * 3) * 12;
		if (adj[ind] == 0) { /* Already in a group. */
			continue;
		}
		const enum edge edge = t.edges[i];
		if (edge == CITY || edge == FIELD) {
			/* All of this edge's other corners are in our group. */
			for (unsigned int k = 1; k < 2; ++k) {
				adj[ind++] = i * 3 + k; /* In a group. */
				adj[(i * 3 + k) * 12] = 0; /*We're its leader */
				adj[(i * 3 + k) * 12 + 1] = i * 3;
			}
		}
		if (edge == ROAD && roadcount != 2) {
			/* Roads are only ever connected if exactly 2 roads. */
			continue;
		}
		if (edge == CITY && center_edge != CITY) {
			/* Cities are only connected by center city */
			continue;
		}
		for (unsigned int j = i + 1; j < 4; ++j) {
			if (t.edges[j] != edge) {
				/* Dissimilar sides not connected. */
				continue;
			}
			for (unsigned int k = 0; k < 3; ++k) {
				if (edge == ROAD) { // 0->2, 1->1, 2->0 (Draw)
					/* Credit to Ben Hammack for finding the
					 * f(x) = 2 - x function (mod 3).
					*/
					int index = i * 3 + k;
					int opposite = j * 3 + (2 - k);
					adj[(index) * 12] = opposite;
					adj[opposite * 12] = 0;
					adj[opposite * 12 + 1] = index;
				} else {
					/* Add whole side to group */
					adj[ind++] = j * 3 + k;
					adj[(j * 3 + k) * 12] = 0;
					adj[(j * 3 + k) * 12 + 1] = i * 3;
				}
			}
		}
	}
}

/* For use with below. */
static int compare_neighbors(size_t *a, size_t *b)
{
	return *a - *b;
}

/* Code here's a little messed up because of void pointers. */
static void merge_sorted_lists(void *merged, size_t *total, 
		void *a, size_t acnt, void *b, size_t bcnt,
		int (*cmp)(void *, void *), size_t elesize)
{
	size_t an, bn, t; 
	an = bn = t = 0;
	while (an < acnt || bn < bcnt) {
		if (an >= acnt) { // Below = merged[t++] = b[bn++] (with void*)
			memcpy((char *)merged + elesize * t++,
					(char *)b + elesize * bn++, elesize);
			continue;
		} else if (bn >= bcnt) {
			memcpy((char *)merged + elesize * t++,
					(char *)a + elesize * an++, elesize);
			continue;
		}
		if (cmp(((char *)a + an * elesize),
					((char *)b + bn * elesize)) < 0) {
			memcpy((char *)merged + elesize * t++,
					(char *)a + elesize * an++, elesize);
		} else if (cmp(((char *)a + an * elesize),
					((char *)b + bn * elesize)) > 0) {
			memcpy((char *)merged + elesize * t++,
					(char *)b + elesize * bn++, elesize);
		} else {
			memcpy((char *)merged + elesize * t++,
					(char *)a + elesize * an++, elesize);
			bn++;
		}
	}
	*total = t;
}

static void merge_features(struct feature *a, struct feature *b)
{
	if (a == b) {
		return;
	} else if (b < a) { /* Swap so a is smaller. */
		struct feature *swap = b;
		b = a;
		a = swap;
	}
	a->weighted_size += b->weighted_size;
	for (size_t i = 0; i < PLAYER_COUNT; ++i) {
		a->meeples[i] += b->meeples[i];
	}
	/* merge neighbors (assumes sorted) */
	size_t total, merged_neighbors[200]; /* 100 + 100 */
	merge_sorted_lists(&merged_neighbors, &total,
			a->neighbors, a->neighbor_count,
			b->neighbors, b->neighbor_count,
			compare_neighbors, sizeof(a->neighbors[0]));
	a->neighbor_count = total;
	memcpy(a->neighbors, merged_neighbors, sizeof(a->neighbors[0]) * total);
	/* merge slot lists (assumes sorted) */
	struct slot merged_slots[100];
	merge_sorted_lists(&merged_slots, &total,
			a->open_slots, a->slot_count,
			b->open_slots, b->slot_count,
			compare_slots, sizeof(a->open_slots[0]));
	a->slot_count = total;
	memcpy(a->open_slots, merged_slots, sizeof(a->open_slots[0]) * total);
	free(b);
}

static size_t get_index(int x, int y, int side, int corner)
{
	return (((x * AXIS + y) * 4) + side) * 3 + corner;
}

/* Refactor to use a vector of features. */
static int create_feature(struct feature **f, enum edge type, struct slot s)
{
	*f = malloc(sizeof(f));
	if (!f) {
		return 1;
	}
	init_feature(*f, type);
	(*f)->slot_count++;
	(*f)->open_slots[0] = s;
	return 0;
}

static void add_adjacency(struct feature *a, size_t ind)
{
	assert(a->neighbor_count < 99);
	size_t i;
	for (i = 0; i < a->neighbor_count; ++i) {
		if (a->neighbors[i] < ind) {
			break;
		}
		if (a->neighbors[i] == ind) {
			return;
		}
	}
	memmove(&a->neighbors[i + 1], &a->neighbors[i],
			sizeof(a->neighbors[i]) * (a->neighbor_count - i));
	a->neighbors[i] = ind;
	a->neighbor_count++;
}

int play_move_feature(struct move m,struct slot **neighbors,struct feature **f)
{
	int adj[12 * 12];
	const int adjs[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
	init_adj(m.tile, adj);
	for (size_t i = 0; i < 12; ++i) { /* Connect corners to placed tiles. */
		if (neighbors[i / 3] == NULL) { /* Neighbor off board. */
			continue;
		}
		const struct slot n = *(neighbors[i / 3]);
		struct feature *matched_feature =f[get_index(n.x, n.y,
				(i / 3 + 2) % 4, 2 - (i % 3))];
		if (matched_feature == NULL) {
			continue;
		}

		size_t lead = i; /* Base case, we're the group leader. */
		if (adj[i * 12] == 0) { /* Not group leader, switch to it. */
			lead = adj[i * 12 + 1];
		}
		for (size_t j = 0; adj[lead * 12 + j] != 0; j++) {
			const int a = adj[lead * 12 + j];
			size_t lead_ind =get_index(m.slot.x, m.slot.y, a/3,a%3);
			if (f[lead_ind]!=NULL && f[lead_ind]!=matched_feature) {
				merge_features(f[lead_ind], matched_feature);
			}
			f[lead_ind] = matched_feature;
		}
	}
	for (size_t i = 0; i < 12; ++i) { /* Assign to unassigned corners */
		const size_t index = get_index(m.slot.x, m.slot.y, i/3, i%3);
		if (f[index] == NULL) {
			/* TODO When creating feature add open slot opposite. */
			if (create_feature(&f[index], m.tile.edges[i / 3],
					make_slot(adjs[i/4][0], adjs[i/4][1]))){
				return 1;
			}
		}
		/* Add tile to each group. */
		if (adj[i * 12] == 0) {
			continue;
		}
		// TODO: add_tile_feature(f[index], adj);
	}
	for (size_t i = 0; i < 12; ++i) { /* add adjencies. */
		const size_t index = get_index(m.slot.x, m.slot.y, i/3, i%3);
		if (adj[i * 12] == 0) { /* Skip group members. */
			continue;
		}
		for (size_t j = 0; j < 12; ++j) { /* Add adjacenct graphs */
			if (j == i || adj[j * 12] == 0) {
				continue;
			}
			const size_t indexp =
				get_index(m.slot.x, m.slot.y, j/3, j%3);
			add_adjacency(f[index], indexp);
			add_adjacency(f[indexp], index);
		}
	}
	return 0;
}

int play_meeple(struct move m, int player, int cnr, struct feature **f)
{
	struct feature *feat = f[get_index(m.slot.x, m.slot.y, cnr/3, cnr%3)];
	assert(feat != NULL);
	if (feat->meeples[player]) {
		return 1; /* Invalid move */
	}
	feat->meeples[player]++;
	return 0;
}
