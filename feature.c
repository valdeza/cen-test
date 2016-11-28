#include "feature.h"

/* Refactor to use a vector of features. */
static struct feature *create_feature(enum edge type, struct slot s)
{
	struct feature *f = malloc(sizeof(*f));
	if (!f) {
		return NULL;
	}
	f->weighted_size = f->neighbor_count = 0;
	for (int i = 0; i < PLAYER_COUNT; ++i) {
		f->meeples[i] = 0;
	}
	f->type = type;
	f->slot_count = 1;
	f->open_slots[0] = s;
	return f;
}

/* General solution. Monasterys are different. Matches up connected graphs */
static void init_adj(struct tile t, int *adj)
{
	memset(adj, 0, sizeof(int) * 12 * 12);
	for (unsigned int i = 0; i < 12; ++i) {
		adj[i * 12] = i + 1; /* nonzero value (assume leader). */
	}
	const enum edge center_edge = t.edges[4];
	for (unsigned int i = 0; i < 4; ++i) {
		const enum edge edge = t.edges[i];
		if (edge != CITY && edge != FIELD) {
			continue;
		}
		if (adj[(i * 3) * 12] == 0) { /* Already in a group. */
			continue;
		}
		unsigned int ind = i * 3 * 12 + 1;
		/* All of this edge's other corners are in our group. */
		for (unsigned int k = 1; k < 3; ++k) {
			adj[ind++] = i * 3 + k + 1; /* In a group. */
			adj[(i * 3 + k) * 12] = 0; /*We're its leader */
			adj[(i * 3 + k) * 12 + 1] = i * 3;
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
				/* Add whole side to group */
				adj[ind++] = j * 3 + k + 1;
				adj[(j * 3 + k) * 12] = 0;
				adj[(j * 3 + k) * 12 + 1] = i * 3;
			}
		}
	}
	for (unsigned int i = 0; i < 4; ++i) {
		const enum edge edge = t.edges[i];
		if (edge != ROAD) {
			continue;
		}

		unsigned int ind = (i * 3 + 0) * 12 + 1;
		if (adj[ind - 1] != 0) {
			for (unsigned int j=(i-1)%4; j!=i; j=(j-1) % 4) {
				if (t.edges[j] != edge) {
					continue;
				}
				adj[ind++] = (j * 3 + 2) + 1;
				adj[(j * 3 + 2) * 12] = 0;
				adj[(j * 3 + 2) * 12 + 1] = i * 3;
				break;
			}
		}

		ind = (i * 3 + 1) * 12 + 1;
		if (adj[ind - 1] != 0) {
			for (unsigned int j=(i + 1)%4; j!=i; j=(j + 1)%4) {
				if (t.edges[j] != edge) {
					continue;
				}
				adj[ind++] = (j * 3 + 1) + 1;
				adj[(j * 3 + 1) * 12] = 0;
				adj[(j * 3 + 1) * 12 + 1] = i * 3 + 1;
			}
		}

		ind = (i * 3 + 2) * 12 + 1;
		if (adj[ind - 1] != 0) {
			for (unsigned int j=(i + 1) % 4; j!=i; j=(j + 1)%4) {
				if (t.edges[j] != edge) {
					continue;
				}
				adj[ind++] = (j * 3 + 0) + 1;
				adj[(j * 3 + 0) * 12] = 0;
				adj[(j * 3 + 0) * 12 + 1] = i * 3 + 2;
				break;
			}
		}
	}
#if 0
				if (edge == ROAD) { // 0->2, 1->1, 2->0 (Draw)
					/* Credit to Ben Hammack for finding the
					 * f(x) = 2 - x function (mod 3).
					*/
					int opposite = j * 3 + (2 - k);
					adj[ind[k]++] = opposite + 1;
					adj[opposite * 12] = 0;
					adj[opposite * 12 + 1] = i * 3 + k;
#if 0
					int index = i * 3 + k;
					int opposite = j * 3 + (2 - k);
					adj[(index) * 12 + 1] = opposite + 1;
					adj[opposite * 12] = 0;
					adj[opposite * 12 + 1] = index;
#endif
#endif
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

static void merge_features(struct feature **ap, struct feature **bp)
{
	struct feature *a = *ap, *b = *bp;
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
			compare_slot_positions, sizeof(a->open_slots[0]));
	a->slot_count = total;
	memcpy(a->open_slots, merged_slots, sizeof(a->open_slots[0]) * total);
	*ap = a;
	*bp = a;
}

static size_t get_index(int x, int y, int side, int corner)
{
	return (((x * AXIS + y) * 4) + side) * 3 + corner;
}

static void merge_nodupes(struct feature **out, struct feature **a, size_t *len)
{
	size_t top = *len;
	size_t count = 0;

	/* Simple insertion sort without dupes. */
	for (size_t i = 0; i < top; ++i) {
		struct feature *f = a[i];
		if (f == NULL) {
			continue; /* No nulls either. */
		}
		size_t j;
		for (j = 0; j < count; ++j) {
			if (out[j] >= f) {
				break;
			}
		}
		if (out[j] == f) {
			continue; /* No dupes. */
		}
		memmove(&out[j + 1], &out[j], sizeof(out[0]) * (count - j));
		out[j] = f;
		count++;
	}
	*len = count;
	return;
}

void update_scores(size_t *scores, struct feature **scratch,
		struct feature **a, size_t *alen)
{
	scores[0] = scores[1] = 0;
	merge_nodupes(scratch, a, alen);
	for (size_t i = 0; i < *alen; ++i) {
		size_t score = 0;
		struct feature *f = scratch[i];
		switch(f->type) {
		case ROAD:
			score = f->weighted_size;
			break;
		case CITY:
			if (f->slot_count == 0) { // Closed
				score = 2 * f->weighted_size;
			} else {
				score = f->weighted_size;
			}
			break;
		case FIELD:
			for (size_t j = 0; j < f->neighbor_count; ++j) {
				if (a[f->neighbors[i]]->type == CITY) {
					score++;
				}
			}
			score *= 3; /* 3 points per neighboring city. */
		case EMPTY:
			continue;
		}
		if (f->meeples[0] >= f->meeples[1]) {
			scores[0] += score;
		}
		if (f->meeples[0] <= f->meeples[1]) {
			scores[1] += score;
		}
	}
}

static void add_adjacency(struct feature *a, size_t ind)
{
	size_t i;
	assert(a->neighbor_count < 99);
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

static void add_tile_feature(struct feature *f, struct tile t)
{
	if (t.attribute == SHIELD) {
		f->weighted_size += 1;
	}
	f->weighted_size += 1;
	return;
}

int play_move_feature(struct move m, struct slot **neighbors,
		struct feature **f, size_t *features_used)
{
	int adj[12 * 12];
	init_adj(m.tile, adj);
	for (size_t i = 0; i < 12; ++i) { /* Connect corners to placed tiles. */
		const struct slot *n = neighbors[i / 3];
		if (n == NULL) { /* Neighbor off board. */
			continue;
		}
		struct feature *companion_feature =
			f[get_index(n->x, n->y, (i/3+2)%4, 2-(i%3))];
		if (companion_feature == NULL) {
			continue;
		}
		const struct feature *dupe = companion_feature;

		size_t lead = i; /* Base case, we're the group leader. */
		if (adj[i * 12] == 0) { /* Not group leader, switch to it. */
			lead = adj[i * 12 + 1];
		}
		printf("DEBUG: lead: %zu\n", lead);
		for (size_t j = 0; adj[lead * 12 + j] != 0; j++) {
			const int a = adj[lead * 12 + j] - 1;
			printf("DEBUG: %d\n", a);
			size_t alt_ind = get_index(m.slot.x, m.slot.y, a/3,a%3);
			if (f[alt_ind]!=NULL && f[alt_ind]!=companion_feature) {
				merge_features(&f[alt_ind], &companion_feature);
			}
			for (size_t k = 0; adj[lead * 12 + k] != 0; ++k) {
				const int b = adj[lead * 12 + k] - 1;
				size_t ind2 =
					get_index(m.slot.x, m.slot.y, a/3, a%3);
				if (f[ind2] == dupe) {
					f[ind2] = companion_feature;
				}
			}
			f[alt_ind] = companion_feature;
		}
	}
	printf("Preassignment:\n");
	for (size_t i = 0; i < 12; ++i) { /* DEBUG: */
		printf("%zu: %p\n", i,
			f[get_index(m.slot.x, m.slot.y, i/3, i%3)]);
	}
	for (size_t i = 0; i < 12; ++i) { /* Assign to unassigned corners */
		if (adj[i * 12] == 0) {
			continue;
		}
		const size_t index = get_index(m.slot.x, m.slot.y, i/3, i%3);
		if (f[index] != NULL) {
			continue;
		}
		/* TODO When creating feature add open slot opposite. */
		int adjs[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
		if (!(f[index] = create_feature(m.tile.edges[i / 3],
			make_slot(m.slot.x + adjs[i/4][0], 
				  m.slot.y + adjs[i/4][1])))) {
			return 1;
		}
		for (size_t j = 0; adj[12 * i + j] != 0; ++j) {
			size_t a = adj[12 * i + j] - 1;
			size_t ip= get_index(m.slot.x,m.slot.y,a/3,a%3);
			f[ip] = f[index];
		}
		*features_used += 1;
	}
	printf("Post Assignment:\n");
	for (size_t i = 0; i < 12; ++i) { /* DEBUG: */
		printf("%zu: %p\n", i,
			f[get_index(m.slot.x, m.slot.y, i/3, i%3)]);
	}
	for (size_t i = 0; i < 12; ++i) {/* add adjencies & tile 2 each group */
		const size_t index = get_index(m.slot.x, m.slot.y, i/3, i%3);
		assert(f[index] != NULL); /* DEBUG */
		if (adj[i * 12] == 0) { /* Skip group members. */
			continue;
		}
		add_tile_feature(f[index], m.tile);
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
	printf("Count: %zu\n", *features_used);
	return 0;
}

/* Refactor to use a vector of features. */
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

#ifdef TEST
#include "game.h"

void print_adj(struct tile t, int *adj)
{
	printf("DEBUG:\n");
	char buf[TILE_LEN];
	printf("%s\n", print_tile(t, buf));
	for (size_t i = 0; i < 12; ++i) {
		if (i + 1 % 12 == 0) {
			printf("\n");
		}
		if (adj[i * 12] == 0) {
			printf("Member of %d group.\n", adj[i * 12 + 1]);
			continue;
		}
		printf("Leader of group. Members: ");
		for (size_t j = 0; j < 12; ++j) {
			printf("%d ", adj[i * 12 + j]);
		}
		printf("\n");
	}
}

int main(void)
{
	int adj[144];
	struct tile t = make_tile((enum edge[5]){ROAD, ROAD, ROAD, ROAD, ROAD}, NONE);
	init_adj(t, adj); print_adj(t, adj);
	t = make_tile((enum edge[5]){CITY, CITY, CITY, CITY, CITY}, SHIELD);
	init_adj(t, adj); print_adj(t, adj);
	t = make_tile((enum edge[5]){CITY, ROAD, FIELD, ROAD, ROAD}, NONE);
	init_adj(t, adj); print_adj(t, adj);

	printf("\n\n Trying game\n");
	struct game g;
	make_game(&g);
	int mid = (AXIS - 1) / 2;
	struct move m = make_move(t, make_slot(mid, mid), 0);
	play_move(&g, m, 0);
	calculate_scores(&g);
	printf("%zu %zu\n", g.scores[0], g.scores[1]);
	m = make_move(t, make_slot(mid + 1, mid), 0);
	play_move(&g, m, 0);
	calculate_scores(&g);
	printf("%zu %zu\n", g.scores[0], g.scores[1]);

	free_game(&g);
	return 0;
}
#endif
