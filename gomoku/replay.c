/** 
 * @file board.c
 * @author Jason Wang
 * This is the main program to replay the gomoku/renju game.
*/
#include <stdio.h>
#include <stdlib.h>
#include "error-codes.h"
#include "game.h"
#include "io.h"

/**
 * This is the main function of the replay function for the game
 * @param argc the number of command line args
 * @param argv an array of command-line argument strings
*/
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: ./replay <saved-match.gmk>\n");
        exit(ARGUMENT_ERR);
    }

    game *g = game_import(argv[1]);
    game_replay(g);
}