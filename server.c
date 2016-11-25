#include <stdlib.h>     /* NULL, malloc(), free() */
#include <stdint.h>	/* uint32_t */
#include <string.h>     /* memset() */
#include <unistd.h>     /* write() */
#include <pthread.h>	/* pthread */

#include <arpa/inet.h>	/* htons */
#include <sys/socket.h> /* bind(), listen(), setsockopt() */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */

#include <stdio.h>	/* DEBUG */
#include <errno.h>	/* errno */
#include "limits.h"	/* AXIS, TILE_SZ */
#include "game.h"	/* Server needs to validate moves. */
#include "serialization.h"

static struct sockaddr_in init_sockaddr(int port)
{
	struct sockaddr_in s;
	memset(&s, '0', sizeof(s));
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = htonl(INADDR_ANY);
	s.sin_port = htons(port);
	return s;
}

/* TODO: Send client hosts with sockets so that 3rd parties can't jump in */
#define PLAYER_COUNT 2

static int send_deck(int *players, size_t pcnt, struct tile *deck, size_t dlen)
{
	/* TODO: Error handling */
	unsigned char buf[TILE_SZ];
	memset(buf, 0, sizeof(buf));
	for (size_t i = 0; i < dlen; ++i) {
		serialize_tile(deck[i], &buf[0]);
		for (size_t j = 0; j < pcnt; ++j) {
			printf("Sending to player %zu: ", j);
			print_buffer(buf, sizeof(buf));
			write(players[j], buf, TILE_SZ);
		}
	}
	return 0;
}

static int send_clock_and_order(int *players, int first, uint64_t seconds)
{
	/* TODO: Error handling */
	struct timeval tm;
	memset(&tm, 0, sizeof(tm));
	tm.tv_sec = (time_t) seconds;

	unsigned char buf[1 + sizeof(seconds)]; // First? + seconds.
	for (size_t i = 0; i < sizeof(seconds); ++i) { /* Serialize seconds */
		buf[i + 1] = (uint8_t) (seconds << (8 * i));
	}

	for (size_t i = 0; i < 2; ++i) {
		if (i == (size_t) first) {
			buf[0] = 1; /* First */
		} else {
			buf[0] = 0; /* Not first */
		}
		printf("Sending clock to player %zu: ", i);
		print_buffer(buf, sizeof(buf));
		write(players[i], buf, sizeof(buf));
		setsockopt(players[i], SOL_SOCKET, SO_RCVTIMEO,
				(char *)&tm, sizeof(tm));
	}

	return 0;
}

enum reason {
	SCORE = 0,
	TIMEOUT = 1,
	INVALID = 2
};

static int game_over(int *players, int winner, enum reason r)
{
	unsigned char buf[1 + TILE_SZ + MOVE_SZ]; // Game_over? + TILE + Move
	buf[0] = 1; /* Game over */
	buf[2] = (uint8_t) r;

	for (size_t i = 0; i < 2; ++i) {
		if (i == (size_t) winner) {
			buf[1] = 1;
			printf("Player %zu has won\n", i);
			printf("DEBUG: Winner | Player: %zu buf: ", i);
		} else {
			buf[1] = 0;
			printf("Player %zu has lost\n", i);
			printf("DEBUG: Loser | Player: %zu buf: ", i);
		}
		print_buffer(buf, sizeof(buf));
		if (write(players[i], buf, sizeof(buf)) != sizeof(buf)) {
			printf("Game over error: %s\n", strerror(errno));
			return 1;
		}
	}
	return 0;
}

/* Step through protocol with clients. */
static void protocol(void *args)
{
	int *hostfd = (int *)args;
	struct game *g = malloc(sizeof(*g));
	make_game(g);
	listen(*hostfd, 10);

	int current_player = 0; /* TODO: Randomly pick player to go first. */
	int players[PLAYER_COUNT] = {0};
	for (size_t i = 0; i < PLAYER_COUNT; ++i) {
		players[i] = accept(*hostfd, NULL, NULL);
	}
	if (send_clock_and_order(players, current_player, 5)) {
		printf("Failed to send clock and order.\n");
	}
	if (send_deck(players, PLAYER_COUNT, g->tile_deck, TILE_COUNT)) {
		printf("Failed to send deck.\n");
	}

	unsigned char buf[1 + TILE_SZ + MOVE_SZ]; // Game_over? + tile + move
	struct move previous;
	while (1) { /* Play game. */
		buf[0] = 0; /* Assume we keep playing. */
		if (is_tile_deck_empty(g)) {
			/* TODO: Scoring. replace 0 with high scoring player. */
			game_over(players, 0, SCORE);
			break;
		}
		struct tile t = deal_tile(g);
		serialize_move(previous, serialize_tile(t, &buf[1]));
		write(players[current_player], buf, sizeof(buf));
		if (read(players[current_player], buf, sizeof(buf))
				< (ssize_t)sizeof(buf)) {
			game_over(players, current_player ^ 1, TIMEOUT);
			break;
		}
		struct move m = deserialize_move(buf);
		if (!is_tile_equal(m.tile, t) || play_move(g, m, current_player)) {
			game_over(players, current_player ^ 1, INVALID);
			break;
		}
		previous = m;
		current_player ^= 1;
	}
	for (int i = 0; i < PLAYER_COUNT; ++i) {
		close(players[i]);
	}
	free(g);
	free(hostfd);
        return;
}

/* Returns the port a socket is listen()ing on in NETWORK ORDER */
static int get_socket_port(int socket)
{
	struct sockaddr_in info;
	memset(&info, 0, sizeof(info));
	socklen_t len = sizeof(info);
	getsockname(socket, (struct sockaddr *)&info, &len);
	return info.sin_port; /* Already in network order. */
}

static int send_ports(int *players, size_t player_count, int port)
{
	/* TODO: Error handling. */
	uint16_t sending_port = port;
	uint8_t buf[sizeof(sending_port)];

	for (size_t i = 0; i < sizeof(buf); ++i) {
		buf[i] = (uint8_t) (port >> (i * 8)); // Serialize ports
	}
	printf("Sending port %u\n", sending_port);

	for (size_t i = 0; i < player_count; ++i) {
		write(players[i], &buf, sizeof(buf));
		close(players[i]); /* No longer need to talk to them. */
	}
	return 0;
}

static int* assign_game_port(void)
{
	int *hostfd = malloc(sizeof(int));	/* Thread will free() */
	assert(hostfd);
	*hostfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(*hostfd >= 0);
	struct sockaddr_in host_addr = init_sockaddr(0); // random port
	bind(*hostfd, (struct sockaddr *)&host_addr, sizeof(host_addr));

        return hostfd;
}

#define LISTEN_PORT 5000 /* Arbitrarily chosen server port. */
int main(void)
{
        struct sockaddr_in serv_addr = init_sockaddr(LISTEN_PORT);

        int listenfd = socket(AF_INET, SOCK_STREAM, 0);
        bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        listen(listenfd, 10);

	int players[PLAYER_COUNT];
	int queued_players = 0;

	pthread_attr_t attr; /* Child opttions TODO REFACTOR */
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 4096*1024); /* 4k Is big enough */

        while (1) {
		/* TODO Ensure unique clients (can't play with self) */
		players[queued_players++] = accept(listenfd, NULL, NULL);
		if (queued_players < PLAYER_COUNT) { /* Wait for match */
			printf("Waiting for match.\n");
			continue;
		}

		/* Found a match. Create a child thread to host game. */
		/* TODO: Lots of error handling. */
		pthread_t *child = malloc(sizeof(pthread_t));
		assert(child);
		pthread_detach(*child);			/* OS will free() */

		int *hostfd = assign_game_port();	/* Thread will free() */
		send_ports(players, PLAYER_COUNT, get_socket_port(*hostfd));
		pthread_create(child, &attr, &protocol, hostfd);
		queued_players = 0;
        }
	close(listenfd);

        return 0;
}
