#include "feature.h"

/* Refactor to use a vector of features. */
static struct feature *create_feature(enum edge type)
{
	struct feature *f = malloc(sizeof(*f));
	if (!f) {
		return NULL;
	}
	f->weighted_size = f->neighbor_count = f->slot_count = 0;
	for (int i = 0; i < PLAYER_COUNT; ++i) {
		f->tigers[i] = f->crocodiles[i] = 0;
	}
	f->type = type;
	return f;
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
		a->tigers[i] += b->tigers[i];
		a->crocodiles[i] += b->crocodiles[i];
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
		case GAMETRAIL:
			score = f->weighted_size;
			break;
		case LAKE:
			if (f->slot_count == 0) { // Closed
				score = 2 * f->weighted_size;
			} else {
				score = f->weighted_size;
			}
			break;
		case JUNGLE:
			for (size_t j = 0; j < f->neighbor_count; ++j) {
				if (a[f->neighbors[i]]->type == LAKE) {
					score++;
				}
			}
			score *= 3; /* 3 points per neighboring city. */
		case EMPTY:
			continue;
		}
		if (f->tigers[0] >= f->tigers[1]) {
			scores[0] += score;
		}
		if (f->tigers[0] <= f->tigers[1]) {
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
	if (t.attribute == PREY) {
		f->weighted_size += 1;
	}
	f->weighted_size += 1;
	return;
}

static int test_corner_feature(int is_tiger, int player,
		struct move m, struct feature **f)
{
	int opp_cnrs[12] = {8, 7, 6, 11, 10, 9, 2, 1, 0, 5, 4, 3};
	int opp_side[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
	int adjs[12 * 12];
	init_adj(m.tile, adjs);
	int check;
	if (is_tiger) {
		check = m.tcorner;
	} else {
		check = m.ccorner;
	}
	if (adjs[check * 12] == 0) {
		check = adjs[check * 12 + 1]; /* Get leader. */
	}
	for (int i = 0; adjs[check * 12 + i] != 0; ++i) {
		const int a = adjs[check * 12 + i] - 1;
		assert(m.ccorner < 12);
		struct feature *opp = f[get_index(
			m.slot.x+ opp_side[a/3][0], m.slot.y+ opp_side[a/3][1],
			opp_cnrs[a] / 3, opp_cnrs[a] % 3)];
		if (!opp) {
			continue;
		}
		if ((m.tcorner >= 0) && (opp->tigers[player] > 0)) {
			return 1;
		}
		if ((m.ccorner >= 0) && (opp->crocodiles[player] > 0)) {
			return 1;
		}
	}
	return 0;
}

int test_meeple(struct move m, int player, struct feature **f)
{
	if (m.tcorner < 0 && m.ccorner < 0) {
		return 0;
	}
	if (m.tcorner >= 0 && m.ccorner >= 0) {
		return 1;
	}
	if (m.tcorner >= 0) {
		if (m.tcorner > 11 && m.tile.attribute != DEN) {
			return 1;
		}
		return test_corner_feature(1, player, m, f);
	} else {
		if (m.ccorner > 11 && m.tile.attribute != DEN) {
			return 1;
		}
		return test_corner_feature(0, player, m, f);
	}
}

static void insert_unless_exists(struct feature *f, struct slot s)
{
	size_t i;
	for (i = 0; i < f->slot_count; ++i) {
		if (compare_slot_positions(s, f->open_slots[i]) > 0) {
			break;
		} else if (compare_slot_positions(s, f->open_slots[i]) == 0) {
			return;
		}
	}
	memcpy(&f->open_slots[i+1], &f->open_slots[i],
			sizeof(f->open_slots[0]) * (f->slot_count - i));
	f->open_slots[i] = s;
	f->slot_count++;
}

int play_move_feature(struct move m, struct slot **neighbors,
		struct feature **f, size_t *features_used)
{
	int adj[12 * 12];
	int opp_corners[12] = {8, 7, 6, 11, 10, 9, 2, 1, 0, 5, 4, 3};
	int opp_side[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
	init_adj(m.tile, adj);

	for (size_t i = 0; i < 12; ++i) { /* Connect corners to placed tiles. */
		const struct slot *n = neighbors[i / 3];
		if (n == NULL) { /* Neighbor off board. */
			continue;
		}
		/* Grab the opposing corner to this one.
		 * Opposing in this case means the corner which this corner
		 * is potentially connected to.
		*/
		const int opp_cnr = opp_corners[i];
		struct feature *opp_feat = /* opp_feature shares connection. */
			f[get_index(n->x, n->y, opp_cnr / 3, opp_cnr % 3)];
		assert(opp_feat != NULL);

		size_t lead = i; /* Base case, we're the group leader. */
		if (adj[i * 12] == 0) { /* Not group leader, switch to it. */
			lead = adj[i * 12 + 1];
		}

		struct feature *lead_feat =
			f[get_index(m.slot.x, m.slot.y, lead / 3, lead % 3)];
		if (lead_feat == NULL) { /* Set the feature. */
			f[lead] = opp_feat;
		} else {
			merge_features(&f[lead], &opp_feat);
		}
	}
	for (size_t i = 0; i < 12; ++i) { /* Assign to unassigned corners */
		if (adj[i * 12] == 0) { /* Only want leaders. */
			continue;
		}
		const size_t index = get_index(m.slot.x, m.slot.y, i/3, i%3);
		if (f[index] != NULL) {
			continue;
		}
		/* TODO When creating feature add open slot opposite. */
		if (!(f[index] = create_feature(m.tile.edges[i/3]))) {
			return 1;
		}
		*features_used += 1;
	}
	for (size_t i = 0; i < 12; ++i) { /* Copy to group members. */
		if (adj[12 * i] == 0) { /* Only grab leaders. */
			continue;
		}
		struct feat *leadf = f[get_index(m.slot.x, m.slot.y, i/3, i%3)];
		for (size_t j = 0; adj[i * 12 + j] != 0; ++j) {
			const int a = adj[i * 12 + j];
			f[get_index(m.slot.x, m.slot.y, a/3, a%3)] = leadf;
		}
	}
	return 0;
}

int play_meeple(struct move m, int player, struct feature **f)
{
	int rc = 0;
	if ((rc = test_meeple(m, player, f))) {
		return rc;
	}
	struct feature *feat = f[get_index(m.slot.x, m.slot.y,
			m.tcorner/3, m.tcorner%3)];
	if (m.tcorner >= 0) {
		feat->tigers[player]++;
	}
	/* TODO: Not right!
	if (m.ccorner > 0) {
		feat->crocodiles[player]++;
	}
	*/
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
	struct tile t = make_tile((enum edge[5]){GAMETRAIL, GAMETRAIL, GAMETRAIL, GAMETRAIL, GAMETRAIL}, NONE);
	init_adj(t, adj); print_adj(t, adj);
	t = make_tile((enum edge[5]){LAKE, LAKE, LAKE, LAKE, LAKE}, PREY);
	init_adj(t, adj); print_adj(t, adj);
	t = make_tile((enum edge[5]){LAKE, JUNGLE, JUNGLE, JUNGLE, JUNGLE}, PREY);
	init_adj(t, adj); print_adj(t, adj);

	printf("\n\n Trying game\n");
	struct game g;
	make_game(&g);

	const int mid = (AXIS - 1) / 2;
	struct move moves[1000];
	size_t max_moves = 1000;

	struct move m = make_move(t, make_slot(mid, mid), 0, 6, -1);
	if (play_move(&g, m, 0)) {
		printf("Success!\n");
	}
	calculate_scores(&g);
	printf("%zu %zu\n", g.scores[0], g.scores[1]);
#if 0
	m = make_move(rotate_tile(t, 1), make_slot(mid + 1, mid), 0, 0, 0);
	if (play_move(&g, m, 0)) {
		printf("Success!\n");
	}
	printf("slot_count: %u\n", g.board.empty_slot_count);
	calculate_scores(&g);
	printf("%zu %zu\n", g.scores[0], g.scores[1]);
	m = make_move(t, make_slot(mid + 1, mid + 1), 0, 6, 0);
	if (play_move(&g, m, 0)) {
		printf("Success!\n");
	}
	printf("slot_count: %u\n", g.board.empty_slot_count);
	t = make_tile((enum edge[5]){LAKE, JUNGLE, JUNGLE, JUNGLE, JUNGLE}, PREY);
#endif
	generate_available_moves(&g, 0, t, &moves, &max_moves);
	for (size_t i = 0; i < max_moves; ++i) {
		printf("x: %d y: %d rotation: %d tcorner: %d\n",
				moves[i].slot.x, moves[i].slot.y,
				moves[i].rotation, moves[i].tcorner);
	}

	free_game(&g);
	return 0;
}
#if 0
int main(void)
{
	int adj[144];
	struct tile t = make_tile((enum edge[5]){LAKE, JUNGLE, JUNGLE, JUNGLE, JUNGLE}, PREY);
	init_adj(t, adj); print_adj(t, adj);
	t = make_tile((enum edge[5]){LAKE, LAKE, LAKE, LAKE, LAKE}, PREY);
	init_adj(t, adj); print_adj(t, adj);
	t = make_tile((enum edge[5]){LAKE, JUNGLE, JUNGLE, GAMETRAIL, GAMETRAIL}, NONE);
	init_adj(t, adj); print_adj(t, adj);

	printf("\n\n Trying game\n");
	struct game g;
	make_game(&g);
	int mid = (AXIS - 1) / 2;
	t = make_tile((enum edge[5]){LAKE, JUNGLE, JUNGLE, JUNGLE, JUNGLE}, PREY);
	struct move m = make_move(t, make_slot(mid, mid), 0, 0, -1);
	play_move(&g, m, 0);
	calculate_scores(&g);
	printf("%zu %zu\n", g.scores[0], g.scores[1]);
	t = make_tile((enum edge[5]){JUNGLE, JUNGLE, LAKE, JUNGLE, JUNGLE}, PREY);
	m = make_move(t, make_slot(mid, mid + 1), 0, 6, -1);
	if (play_move(&g, m, 0)) {
		printf("Success!\n");
	}
	calculate_scores(&g);
	printf("%zu %zu\n", g.scores[0], g.scores[1]);

	free_game(&g);
	return 0;
}
#endif
#endif
