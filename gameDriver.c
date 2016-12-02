#include <stdlib.h>

#include <assert.h>

#include "game.h"
#include "game.h"

int main(){
 /* int graph[100] = {2};
  board setBoard = make_board();
  enum edge edges[5];
  edges[0] = CITY;     //1
  edges[1] = CITY;      //1
  edges[2] = CITY;     //1
  edges[3] = ROAD;      //3
  edges[4] = CITY;      //1
  enum attribute attribute = SHIELD;  //1
  tile setTile = make_tile(edges, attribute);


  int graphUsed = 5;
  int tileUsed = 5;
  int score[2] = {23, 24};*/
  //Test make_game function
  struct game games;
  make_game(&games);
  printf("Excepted result for tiles_used: 0\n");
  printf("Actual result for tiles_used: %i\n", games.tiles_used);
  printf("Excepted result for scores[0]: 0\n");
  printf("Actual result for scores[0]: %i\n", games.scores[0]);
  printf("Excepted result for scores[1]: 0\n");
  printf("Actual result for scores[1]: %i\n", games.scores[1]);
  assert(0 == games.tiles_used);
  assert(0 == games.scores[0]);
  assert(0 == games.scores[1]);

  //I dont know how to test array with size 72
  //graph array
  printf("make_game function works fine\n\n");

  //Test function set_game_deck
  enum edge edges[5];
  edges[0] = CITY;     //1
  edges[1] = CITY;      //1
  edges[2] = CITY;     //1
  edges[3] = ROAD;      //3
  edges[4] = CITY;      //1
  enum attribute attribute = SHIELD;  //1
  struct tile tileArray[TILE_COUNT];
  for(int i = 0; i < TILE_COUNT; i++){
    tileArray[i] = make_tile(edges, attribute);
  }
  set_game_deck(&games, tileArray);
  for(int i = 0; i < TILE_COUNT; i++){
    assert(games.tile_deck[i].edges[0] == 1);
    assert(games.tile_deck[i].edges[1] == 1);
    assert(games.tile_deck[i].edges[2] == 1);
    assert(games.tile_deck[i].edges[3] == 3);
    assert(games.tile_deck[i].edges[4] == 1);
    assert(games.tile_deck[i].attribute == 1);
  }
  printf("Function set_game_deck works fine\n");

  //Test play_move function
  struct tile setTile = make_tile(edges, attribute);
  int x = 2;
  int y = 3;
  struct slot slotX = make_slot(x, y);
  int rotation = 2;
  struct move setMove = make_move(setTile, slotX, rotation, -1, -1);
  int player = 2;
  printf("Excepted result for play_move: 1\n");// invalid move
  printf("Actual result for play_move: %i\n", play_move(&games, setMove, player));
  assert(play_move(&games, setMove, player) == 1);
  printf("play_move function works fine\n");

  //Test deal_tile function
  enum edge edges2[5];
  edges2[0] = ROAD;     //3
  edges2[1] = ROAD;      //3
  edges2[2] = ROAD;     //3
  edges2[3] = ROAD;      //3
  edges2[4] = ROAD;      //3
  attribute = SHIELD;  //3
  struct tile tile1 = make_tile(edges2, attribute);
  games.tile_deck[0] = tile1;
  struct tile tile2 = make_tile(edges, attribute);
  games.tile_deck[1] = tile2;
  games.tiles_used = 0;
  struct tile currentTile = deal_tile(&games);
  assert(currentTile.edges[0] == edges2[0]);
  assert(currentTile.edges[1] == edges2[1]);
  assert(currentTile.edges[2] == edges2[2]);
  assert(currentTile.edges[3] == edges2[3]);
  assert(currentTile.edges[4] == edges2[4]);
  assert(currentTile.attribute == attribute);
  currentTile = deal_tile(&games);
  assert(currentTile.edges[0] == edges[0]);
  assert(currentTile.edges[1] == edges[1]);
  assert(currentTile.edges[2] == edges[2]);
  assert(currentTile.edges[3] == edges[3]);
  assert(currentTile.edges[4] == edges[4]);
  assert(currentTile.attribute == attribute);
  printf("Function deal_tile works fine\n");
}


