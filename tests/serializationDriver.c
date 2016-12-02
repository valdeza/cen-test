#include <stdlib.h>

#include <assert.h>

#include "serialization.h"
#include "serialization.h"

int main(){

  //Test function serialize_tile
  printf("Test function serialize_tile\n");
  enum edge edges[5];
  edges[0] = EMPTY;     //0
  edges[1] = CITY;      //1
  edges[2] = FIELD;     //2
  edges[3] = ROAD;      //3
  edges[4] = CITY;      //1
  enum attribute attributes = SHIELD;  //1
  struct tile setTile = make_tile(edges, attributes);
  uint8_t buf[100];
  for(int i = 0; i < 100; i++){
    buf[i] = 100;
  }

  printf("Expected result: 0\n");
  printf("Actual result: %i\n", *serialize_tile(setTile, buf));
  assert (*serialize_tile(setTile, buf) == 100);
  printf("Function serialize_tile works fine\n\n");
  //Test function deserialize_tile
  printf("\nTest function deserialize_tile\n");
  struct tile anotherTile = deserialize_tile(buf);
printf("Expected result for edge is\n");
  printf("edges[0]: %i \t", buf[0]);
  printf("edges[1]: %i \t", buf[1]);
  printf("edges[2]: %i \t", buf[2]);
  printf("edges[3]: %i \t", buf[3]);
  printf("edges[4]: %i \t", buf[4]);
  printf("\nActual value for edge is\n");
  printf("edges[0]: %i \t" , anotherTile.edges[0]);
  printf("edges[1]: %i \t" , anotherTile.edges[1]);
  printf("edges[2]: %i \t" , anotherTile.edges[2]);
  printf("edges[3]: %i \t" , anotherTile.edges[3]);
  printf("edges[4]: %i \t" , anotherTile.edges[4]);
  assert(anotherTile.edges[0] == buf[0]);
  assert(anotherTile.edges[1] == buf[1]);
  assert(anotherTile.edges[2] == buf[2]);
  assert(anotherTile.edges[3] == buf[3]);
  assert(anotherTile.edges[4] == buf[4]);

  printf("\nExpected result for attribute: %i\n", buf[5]);
  printf("Actual result for attribute: %i\n", anotherTile.edges[5]);
  assert(buf[5] == anotherTile.edges[5]);
  printf("Function deserialize_tile works fine\n\n");


  //Test function serialize_move
  printf("Test function serialize_move\n");
  int x = 2;
  int y = 3;
  struct slot slotX = make_slot(x, y);
  int rotation = 2;
  struct move setMove = make_move(setTile, slotX, rotation, -1, -1);
  printf("Expected result: 0\n");
  printf("Actual result: %i\n", *serialize_move(setMove, buf));
  assert(*serialize_move(setMove, buf) == 100);
  printf("Function serialize_move works fine\n\n");

  //Test function deserialize_move
  printf("Test function deserialize_move\n");
  setMove = deserialize_move(buf);
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

  printf("\nExpected result for attribute: %i\n", attributes);
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
  printf("deserialize_move function works fine\n\n");

  int length = 0;
  for(int i = 0; i < 100; i++){
    if(buf[i] == 100) break;
    length++;
  }

  printf("Print out the value of buf:\n");
  print_buffer(buf, length);
}
