/* TODO Test tcorner, ccorner */

#include <stdlib.h>

#include <assert.h>

#include "move.h"
#include "move.h"

int main(){
  printf("Test function make_move\n");
  enum edge edges[5];
  edges[0] = EMPTY;     //0
  edges[1] = CITY;      //1
  edges[2] = FIELD;     //2
  edges[3] = ROAD;      //3
  edges[4] = CITY;      //1

  enum attribute attribute = SHIELD;  //1
  struct tile setTile = make_tile(edges, attribute);
  int x = 2;
  int y = 3;
  struct slot slotX = make_slot(x, y);
  int rotation = 2;
  struct move setMove = make_move(setTile, slotX, rotation, -1, -1);
  printf("Expected result is\n");
  printf("edges[0]: %i \t", edges[0]);
  printf("edges[1]: %i \t", edges[1]);
  printf("edges[2]: %i \t", edges[2]);
  printf("edges[3]: %i \t", edges[3]);
  printf("edges[4]: %i \t", edges[4]);
  printf("\nActual value is\n");
  printf("edges[0]: %i \t" , setMove.tile.edges[0]);
  printf("edges[1]: %i \t" , setMove.tile.edges[1]);
  printf("edges[2]: %i \t" , setMove.tile.edges[2]);
  printf("edges[3]: %i \t" , setMove.tile.edges[3]);
  printf("edges[4]: %i \t" , setMove.tile.edges[4]);

  printf("\nExpected result for attribute: %i\n", attribute);
  printf("Actual result for attribute: %i\n", setMove.tile.attribute);

  printf("Expected result for slot x: %i\ty: %i\n",x, y);
  printf("Actual result for slot x: %i\ty: %i\n",setMove.slot.x, setMove.slot.y);

  printf("Expected result for rotation: %i\n",rotation);
  printf("Actual result for rotation: %i\n",setMove.rotation);

  assert(setMove.tile.edges[0] == setTile.edges[0]);
  assert(setMove.tile.edges[1] == setTile.edges[1]);
  assert(setMove.tile.edges[2] == setTile.edges[2]);
  assert(setMove.tile.edges[3] == setTile.edges[3]);
  assert(setMove.tile.edges[4] == setTile.edges[4]);
  assert(setMove.tile.attribute == setTile.attribute);
  assert(setMove.slot.x == slotX.x);
  assert(setMove.slot.y == slotX.y);
  assert(setMove.rotation == 2);
  printf("make_move function works fine");
}
