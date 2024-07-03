/** 
 * @file board.c
 * @author Jason Wang
 * This program utilizes functions to manage board structs and control the board.
*/
#include "board.h"
#include "error-codes.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
/**
 * This function creates a new dynamically allocated board struct, initializes board.size with the parameter size, 
 * initializes board.grid with a new dynamically allocated array, initializes all grid intersections with EMPTY_INTERSECTION, 
 * finally it returns the struct created.
 * If an invalid size is given, exit with the code BOARD_SIZE_ERR as defined in  error-codes.h
 * @param size the size of the board
 * @return board structure 
*/
board* board_create(unsigned char size) {
    if (!(size == 15 || size == 17 || size == 19)) {
        exit(BOARD_SIZE_ERR);
    }
    board *newBoard = (board *) malloc(sizeof(board));
    if (!newBoard) {
        return NULL;
    }
    newBoard->size = size;
    newBoard->grid = (unsigned char *) malloc(size * size * sizeof(unsigned char));
    if (!newBoard->grid) {
        free(newBoard);
        return NULL;
    }
    for (int i = 0; i < size * size; i++) {
        newBoard->grid[i] = EMPTY_INTERSECTION;
    }
    return newBoard;
}

/**
 * This function frees the memory of a dynamically allocated board struct, it also frees the memory of its dynamically allocated grid field.
 * If the pointer b is NULL, exit with the code NULL_POINTER_ERR as defined in error-codes.h.
 * @param b the board to free space from
*/
void board_delete(board* b) {
    if (!b) {
        exit(NULL_POINTER_ERR);
    }
    free(b->grid);
    free(b);
}

/**
 * This function prints a the board in the format specified
 * If in_place is true, it clears the terminal first
 * @param b the board to print
 * @param in_place clears terminal if true.
*/
void board_print(board* b, bool in_place) {
    if (in_place) {
        clear();
    }
    for (int i = b->size - 1; i >= 0; i--) {
        for (int j = 0; j < b->size; j++) {
            if (j == 0) {
                printf("%2d ", i + 1);
            }
            if (j == b->size - 1) {
                switch (b->grid[b->size * i + j]) {
                    case EMPTY_INTERSECTION: printf("+"); break;
                    case BLACK_STONE: printf("\u25CF"); break;
                    case WHITE_STONE: printf("\u25CB"); break;
                    default: break;
                }
                printf("\n");
            } else {
                switch (b->grid[b->size * i + j]) {
                    case EMPTY_INTERSECTION: printf("+"); break;
                    case BLACK_STONE: printf("\u25CF"); break;
                    case WHITE_STONE: printf("\u25CB"); break;
                    default: break;
                }
                printf("-");
            }
        }
    }
    printf("   ");
    for (int j = 0; j < b->size; j++) {
        if (j == b->size - 1) {
            printf("%c\n", 'A' + j);
        } else {
            printf("%c ", 'A' + j);
        }
    }
}

/**
 * This function converts the horizontal coordinate x and vertical coordinate y for a  board.grid to a "letter + number" formal coordinate,
 * and stores the result in the buffer  formal_coord. Finally it returns SUCCESS.
 * If the x and y coordinates are invalid for board b,
 * return  COORDINATE_ERR instead. Return codes are defined in error-codes.h.
 * @param b the board
 * @param x the horizontal coordinate
 * @param y the vertical coordinate
 * @param formal_coord the buffer
 * @return unsigned char for the success.
*/
unsigned char board_formal_coord(board* b, unsigned char x, unsigned char y, char* formal_coord) {
    if (x < 'A' || x > b->size - 1 + 'A' || y < 1 || y > b->size) {
        return COORDINATE_ERR;
    }
    unsigned char letter = x;
    unsigned char number = y;
    sprintf(formal_coord, "%c%d", letter, number);

    return SUCCESS;
}

/**
 * This function converts a "letter + number" formal coordinate string formal_coord to the horizontal and vertical coordinates for a board.grid, 
 * and stores the results in x and  y that are passed by reference. Finally it returns SUCCESS.
 * If formal_coord is invalid for board b
 * return FORMAL_COORDINATE_ERR instead. Return codes are defined in error-codes.h.
 * @param b the board
 * @param formal_coord the stored x/y coordinates
 * @param x the horizontal coordinate
 * @param y the vertical coordinate
 * @return return codes
*/
unsigned char board_coord(board* b, const char* formal_coord, unsigned char* x, unsigned char* y) {
    unsigned char letter;
    char numberStr[10];
    if (sscanf(formal_coord, "%c%s", &letter, numberStr) != 2) {
        return FORMAL_COORDINATE_ERR;
    }
    if (letter < 'A' || letter > 'A' + b->size - 1) {
        return FORMAL_COORDINATE_ERR;
    }
    int number = atoi(numberStr);
    if (number < 1 || number > b->size) {
        return FORMAL_COORDINATE_ERR;
    }
    *x = letter;
    *y = number;

    return SUCCESS;
}

/**
 * This function returns the intersection occupation state stored in a board.grid at the given horizontal and vertical coordinate pair x and y.
 * @param b the board
 * @param x the horizontal coordinate
 * @param y the vertical coordinates
 * @return the item at the location.
*/
unsigned char board_get(board* b, unsigned char x, unsigned char y) {
    int col = x - 'A';
    int row = y - 1;
    return b->grid[row * b->size + col];
}

/**
 * This function stores the intersection occupation state stone to a board.grid at the given horizontal and vertical coordinate pair x and y.
 * If stone is neither BLACK_STONE or WHITE_STONE, exit with the code  STONE_TYPE_ERR as defined in error-codes.h.
 * @param b the board
 * @param x the horizontal coordinates
 * @param y the vertical coordinates
 * @param stone the color of the stone
*/
void board_set(board* b, unsigned char x, unsigned char y, unsigned char stone) {
    if (!(stone == BLACK_STONE || stone == WHITE_STONE)) {
        exit(STONE_TYPE_ERR);
    }
    int col = x - 'A';
    int row = y - 1;
    b->grid[row * b->size + col] = stone;
}

/**
 * This function returns true if all intersections of a board.grid is occupied by a stone, otherwise it returns false.
 * @param b the board
 * @return true or false based on whether the board is full or not.
*/
bool board_is_full(board* b) {
    for (int i = 0; i < b->size * b->size; i++) {
        if (b->grid[i] == EMPTY_INTERSECTION) {
            return false;
        }
    }
    return true;
}