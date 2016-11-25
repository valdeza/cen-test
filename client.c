#include <errno.h>	/* errno */
#include <stdio.h>	/* printf() */
#include <stdint.h>	/* uint32_t */
#include <string.h>	/* memset(), strerror() */
#include <unistd.h>	/* read(), write() */

#include <arpa/inet.h>  /* inet_pton() */

#include <sys/socket.h> /* socket() */
#include <sys/uio.h>	/* read(), write() */
#include <sys/types.h>	/* read(), write() */

#include "limits.h"
#include "serialization.h"
#include "game.h"
#include "move.h"

static struct sockaddr_in make_sockaddr_in_port(int port)
{
	struct sockaddr_in s;
	memset(&s, '0', sizeof(s));
	s.sin_family = AF_INET;
	s.sin_port = port;
	return s;
}

static int retry_connection(int sock, struct sockaddr *a, size_t asize, int cnt)
{
	for (int i = 0; i < cnt; ++i) {
		if (!connect(sock, a, asize)) {
			return 0;
		}
		switch (errno) {
		case ECONNREFUSED:
		case EHOSTUNREACH:
		case ENETDOWN:
		case ENETUNREACH:
		case ETIMEDOUT:
		case ECONNRESET:
			break;	/* retry */
		default:	/* can't fix, so bail out */
			return 1;
		}
		sleep(1);
	}
	return 1;
}

static int connect_retry(char *host, int port)
{
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Could not create socket: %s\n", strerror(errno));
		return -1;
	}

	struct sockaddr_in s = make_sockaddr_in_port(port);
	if (!inet_pton(AF_INET, host, &s.sin_addr)) {
		printf("Bad address.\n");
		return -1;
	} 

	if (retry_connection(sockfd, (struct sockaddr *)&s, sizeof(s), 10)) {
		printf("Connection Failure: %s\n", strerror(errno));
		return -1;
	}

	return sockfd;
}

static int connect_game(char *host, int welcome_port)
{
	/* TODO: Better error handling. */
	int sockfd;
	if ((sockfd = connect_retry(host, htons(welcome_port))) < 0) {
		printf("Error: %s\n", strerror(errno));
		return -1;
	} 
	uint16_t port = 0;
	unsigned char buf[sizeof(port)];
	if (read(sockfd, &buf, sizeof(buf)) < 0) {
		printf("Read error: %s\n", strerror(errno));
		return -1;
	}
	for (size_t i = 0; i < sizeof(buf); ++i) {
		port += (buf[i] << (8 * i));
	}
	printf("Got port: %u\n", ntohs(port));
	close(sockfd);
	if ((sockfd = connect_retry(host, port)) < 0) {
		printf("Error: %s\n", strerror(errno));
		return -1;
	}
	return sockfd;
}

static int get_clock_and_order(int sockfd, int *first, uint64_t *clock)
{
	unsigned char buf[1 + sizeof(*clock)]; // first? + clock
	read(sockfd, buf, sizeof(buf));
	print_buffer(buf, sizeof(buf));
	*first = buf[0];
	*clock = 0;
	for (size_t i = 0; i < sizeof(buf) - 1; ++i) {
		*clock += (buf[i + 1] << (i * 8));
	}
	return 0;
}

static int get_deck(int sockfd, struct tile *deck, size_t clen, size_t dlen)
{
	unsigned char buf[clen];
	for (size_t i = 0; i < dlen; ++i) {
		read(sockfd, buf, clen);
		enum edge edges[5];
		for (size_t j = 0; j < 5; ++j) {
			edges[j] = buf[j];
		}
		enum attribute a = buf[5];
		deck[i] = make_tile(edges, a);
	}
	return 0;
}

#define REMOTE_HOST "127.0.0.1" /* TODO: Get a command line variable. */
#define REMOTE_PORT 5000 /* TODO: Factor into command line variable. */

static struct game *init_game(int socket)
{
	/* TODO: Error handling? */
	struct game *g = malloc(sizeof(*g));
	struct tile *tileset = malloc(sizeof(*tileset) * TILE_COUNT);
	get_deck(socket, tileset, TILE_SZ, TILE_COUNT);
	set_game_deck(g, tileset);
	free(tileset);
	return g;
}

int main(void)
{
	int sockfd;
	if ((sockfd = connect_game(REMOTE_HOST, REMOTE_PORT)) < 0) {
		printf("Error: %s\n", strerror(errno));
		return 1;
	}
	printf("Successfully connected.\n");

	int first;
	uint64_t move_clock;
	get_clock_and_order(sockfd, &first, &move_clock);
	printf("Clock: %llu\n", move_clock);
	if (first) {
		printf("I'm first!\n");
	} else {
		printf("I'm second!\n");
	}

	struct game *g = init_game(sockfd); /* TODO: Refactor? */
	unsigned char buf[1 + TILE_SZ + MOVE_SZ]; // game_over? + tile + move
	while (read(sockfd, buf, sizeof(buf)) == sizeof(buf)) {
		printf("Recieved: ");
		print_buffer(buf, sizeof(buf));
		if (buf[0]) { /* game over. */
			if (buf[1]) {
				printf("I won!\n");
			} else {
				printf("I lost!\n");
			}
			break;
		}
		/* Deserialize tile and move. */
		struct tile t = deserialize_tile(&buf[1]);
		unsigned char b[100];
		printf("Tile: \n%s\n", print_tile(t, b));
		if (!first) {
			struct move prev = deserialize_move(&buf[7]);
			printf("Prev move | x: %d y: %d: rotation: %d \n%s\n",
				prev.slot.x, prev.slot.y, prev.rotation,
				print_tile(prev.tile, b));
			play_move(g, prev, 1);
		} else { /* No previous move to deal with. */
			first = 0;
		}
		int mid = (AXIS - 1) / 2;
		struct move m = make_move(t, make_slot(mid, mid), 0);
		play_move(g, m, 0);
		serialize_move(m, buf);
		printf("Try playing the center.\n");
		write(sockfd, buf, sizeof(buf));
	}
	close(sockfd);
	free(g);
	return 0;
}
