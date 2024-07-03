#ifndef _BOARD_H_
#define _BOARD_H_
#include <stdbool.h>
#define EMPTY_INTERSECTION 0
#define BLACK_STONE 1
#define WHITE_STONE 2
#define clear() printf("\033[H\033[J")

typedef struct {
    unsigned char size;
    unsigned char* grid;
} board;

/** function to create a board */
board* board_create(unsigned char size);
/** function to delete a board */
void board_delete(board* b);
/** function to print a board */
void board_print(board* b, bool in_place);
/** function to calculate coord a board */
unsigned char board_formal_coord(board* b, unsigned char x, unsigned char y, char* formal_coord);
/** function to help calculate coord for board */
unsigned char board_coord(board* b, const char* formal_coord, unsigned char* x, unsigned char* y);
/** function to get a board */
unsigned char board_get(board* b, unsigned char x, unsigned char y);
/** function to set a piece a board */
void board_set(board* b, unsigned char x, unsigned char y, unsigned char stone);
/** function to check if board is full */
bool board_is_full(board* b);
#endif
