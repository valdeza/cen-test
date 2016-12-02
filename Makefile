CFLAGS=-std=c99 -g -march=native -flto -Wall -Wextra -pedantic -O0

.PHONY: all tests clean

all: test_game test_board server client tests

clean:
	rm *.o
	rm client server
	rm test_game test_board test_feature \
		boardDriver gameDriver moveDriver serializationDriver slotDriver \
		tileDriver

tests: boardDriver moveDriver serializationDriver slotDriver tileDriver \
	gameDriver

boardDriver: boardDriver.c board.o slot.o tile.o move.o

moveDriver: moveDriver.c slot.o tile.o move.o

serializationDriver: serializationDriver.c serialization.o slot.o tile.o move.o

slotDriver: slotDriver.c slot.o

tileDriver: tileDriver.c tile.o

gameDriver: gameDriver.c game.o board.o slot.o tile.o move.o rng.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

server: server.c game.o rng.o tile.o board.o slot.o serialization.o
	$(CC) $(CFLAGS) -o server server.c game.o rng.o tile.o move.o board.o \
		slot.o serialization.o -lm -pthread

client: client.c game.o rng.o tile.o board.o slot.o serialization.o
	$(CC) $(CFLAGS) -o client client.c game.o rng.o tile.o move.o board.o \
		slot.o serialization.o -lm

test_game: game.c game.h rng.o tile.o board.o slot.o
	$(CC) $(CFLAGS) -DTEST -o test_game game.c rng.o tile.o board.o slot.o \
		-lm

test_board: board.c board.h tile.o slot.o move.o
	$(CC) $(CFLAGS) -DTEST -o test_board board.c tile.o slot.o move.o

serialization.o: serialization.c serialization.h
	$(CC) $(CFLAGS) -c -o serialization.o serialization.c

game.o: game.c game.h
	$(CC) $(CFLAGS) -c -o game.o game.c

board.o: board.c board.h tile.o slot.o move.o
	$(CC) $(CFLAGS) -c -o board.o board.c

rng.o: rngs/mt19937-64.c rngs/mt19937-64.h
	$(CC) $(CFLAGS) -c -o rng.o rngs/mt19937-64.c

move.o: move.c move.h
	$(CC) ${CFLAGS} -c -o move.o move.c

tile.o: tile.c tile.h
	$(CC) ${CFLAGS} -c -o tile.o tile.c

slot.o: slot.c slot.h
	$(CC) ${CFLAGS} -c -o slot.o slot.c
