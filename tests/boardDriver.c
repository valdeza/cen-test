#include <stdlib.h>

#include <assert.h>

#include "board.h"
#include "slot.h"

int main(){
  int x = 3;
  int y = 5;
  struct slot setSlot = make_slot(x,y);
  struct board setBoard = make_board();

  enum edge edges[5];
  edges[0] = LAKE;      //1
  edges[1] = LAKE;      //1
  edges[2] = LAKE;      //1
  edges[3] = GAMETRAIL; //3
  edges[4] = LAKE;      //1
  enum attribute attribute = PREY; //1
  struct tile setTile = make_tile(edges, attribute);
  int rotation = 2;
  struct move setMove = make_move(setTile, setSlot, rotation, -1, -1);

  enum edge edges2[5];
  edges2[0] = LAKE;   //1
  edges2[1] = JUNGLE; //2
  edges2[2] = JUNGLE; //2
  edges2[3] = JUNGLE; //2
  edges2[4] = JUNGLE; //2
  struct tile setEmptyTile = make_tile(edges2, attribute);
  struct move setEmptyMove = make_move(setTile, setSlot, rotation, -1, -1);

  //Test function make_board
  {
    printf("Test Make board function\n");\
    printf("Excepted result for tile: no tile\n");
    printf("Actual result for tile: ");
    if(setBoard.tiles[0].edges[0] == 0)
      printf("no tile\n");
    else
      printf("there are some tiles\n");

    printf("Excepted result for empty_slot_count: 1\n");
    printf("Actual result for empty_slot_count: %i\n",setBoard.empty_slot_count);

    printf("Excepted result for slot_spots[0]: \n");
    printf("slot_spots[0].x: %i\tslot_spots[0].y: %i\n", (AXIS - 1) / 2, (AXIS - 1) / 2);
    printf("Actual result for slot_spots[0]: \n");
    printf("slot_spots[0].x: %i\tslot_spots[0].y: %i\n", setBoard.slot_spots[0].x, setBoard.slot_spots[0].y);

    assert(setBoard.tiles[0].edges[0] == 0);  // To test weather board has tiles or not
    assert(setBoard.empty_slot_count == 1);
    assert(setBoard.slot_spots[0].x == (AXIS - 1) / 2);
    assert(setBoard.slot_spots[0].y == (AXIS - 1) / 2);
    printf("Function make_board works fine\n\n");
  }

  //Test function print_board
  /*{
    char a [BOARD_LEN];
    cout << "Test function print_board" << endl;   // not done
    cout << print_board(setBoard,a );
  }*/

  //Test function play_move_board
 {
    printf("Test function play_move_board\n");
    struct board *pointToBoard = &setBoard;
    int a = play_move_board(pointToBoard, setMove);
    printf("Expected result is: 1\n");
    printf("Actual result is : %i\n", a);
    assert(a == 1);
    printf("Function play_move_board works fine\n");
  }
}
