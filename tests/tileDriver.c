#include <stdlib.h>

#include <assert.h>

#include "edge.h"
#include "edge.h"
#include "tile.h"
#include "tile.h"


int main(){

  enum edge edges[5];
  edges[0] = EMPTY;     //0
  edges[1] = CITY;      //1
  edges[2] = FIELD;     //2
  edges[3] = ROAD;      //3
  edges[4] = CITY;      //1

  enum attribute attributes = SHIELD;  //1

  //Make tile Function
  printf("Test make_tile function\n");
  struct tile setTile = make_tile(edges, attributes);
  printf("Expected result is\n");
  printf("edges[0]: %i \t", edges[0]);
  printf("edges[1]: %i \t", edges[1]);
  printf("edges[2]: %i \t", edges[2]);
  printf("edges[3]: %i \t", edges[3]);
  printf("edges[4]: %i \t", edges[4]);
  printf("\nActual value is\n");
  printf("edges[0]: %i \t" , setTile.edges[0]);
  printf("edges[1]: %i \t" , setTile.edges[1]);
  printf("edges[2]: %i \t" , setTile.edges[2]);
  printf("edges[3]: %i \t" , setTile.edges[3]);
  printf("edges[4]: %i \t" , setTile.edges[4]);
  assert(setTile.edges[0] == 0);
  assert(setTile.edges[1] == 1);
  assert(setTile.edges[2] == 2);
  assert(setTile.edges[3] == 3);
  assert(setTile.edges[4] == 1);
  printf("\nExpected result for attribute: %i" , attributes);
  printf("\nActual result for attribute: %i" , attributes);
  assert(setTile.attribute == 1);
  printf("\nmake_tile function works fine\n\n");

  printf("Test rotate_tile function\n");
  struct tile rotateTile = rotate_tile(setTile, 2);
  printf("Expected result is\n");
  printf("edges[0]: %i \t" , edges[0]);
  printf("edges[1]: %i \t" , edges[1]);
  printf("edges[2]: %i \t" , edges[2]);
  printf("edges[3]: %i \t" , edges[3]);
  printf("edges[4]: %i \t" , edges[4]);
  printf("\nActual value is\n");
  printf("edges[0]: %i \t" , rotateTile.edges[0]);
  printf("edges[1]: %i \t" , rotateTile.edges[1]);
  printf("edges[2]: %i \t" , rotateTile.edges[2]);
  printf("edges[3]: %i \t" , rotateTile.edges[3]);
  printf("edges[4]: %i \t" , rotateTile.edges[4]);
  assert(rotateTile.edges[0] == 2);
  assert(rotateTile.edges[1] == 3);
  assert(rotateTile.edges[2] == 0);
  assert(rotateTile.edges[3] == 1);
  assert(rotateTile.edges[4] == 1);
  printf("\nrotate_tile function works fine\n");

  char b[12];
  char* print = print_tile(setTile, b);
  printf("\noriginal tile: \n %s" , print_tile(setTile, b));
  assert(print[0] == '+');
  assert(print[1] == 'X');
  assert(print[2] == '+');
  assert(print[3] == '\n');
  assert(print[4] == 'R');
  assert(print[5] == 'C');
  assert(print[6] == 'C');
  assert(print[7] == '\n');
  assert(print[8] == '+');
  assert(print[9] == 'F');
  assert(print[10] == '+');
  assert(print[11] == '\n');
  printf("print_tile function works fine\n");
}
