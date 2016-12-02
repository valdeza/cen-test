CFLAGS=-std=c99 -g -march=native -flto -Wall -Wextra -pedantic -O0 -I ..
OBJ_DIR=..

.PHONY: all clean

all: boardDriver moveDriver serializationDriver slotDriver tileDriver \
	gameDriver

clean:
	$(RM) boardDriver gameDriver moveDriver serializationDriver slotDriver \
		tileDriver

boardDriver: boardDriver.c $(OBJ_DIR)/board.o $(OBJ_DIR)/slot.o \
	$(OBJ_DIR)/tile.o $(OBJ_DIR)/move.o

moveDriver: moveDriver.c $(OBJ_DIR)/slot.o $(OBJ_DIR)/tile.o $(OBJ_DIR)/move.o

serializationDriver: serializationDriver.c $(OBJ_DIR)/serialization.o \
	$(OBJ_DIR)/slot.o $(OBJ_DIR)/tile.o $(OBJ_DIR)/move.o

slotDriver: slotDriver.c $(OBJ_DIR)/slot.o

tileDriver: tileDriver.c $(OBJ_DIR)/tile.o

gameDriver: gameDriver.c $(OBJ_DIR)/game.o $(OBJ_DIR)/board.o \
	$(OBJ_DIR)/slot.o $(OBJ_DIR)/tile.o $(OBJ_DIR)/move.o $(OBJ_DIR)/feature.o \
	$(OBJ_DIR)/rng.o
	$(CC) $(CFLAGS) -o $@ $^ -lm