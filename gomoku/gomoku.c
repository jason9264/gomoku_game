/** 
 * @file board.c
 * @author Jason Wang
 * This is the main program to play the gomoku game.
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "error-codes.h"
#include "board.h"
#include "game.h"
#include "io.h"

#define DEFAULT_SIZE 15

/**
 * The main function of the gomoku game
 * @param argc the number of commandline args
 * @param argv an array of command line argument strings
 * @return exit status of the program
*/
int main(int argc, char *argv[]) {
    int opt;
    char *options = "o:r:b:";
    char outputFile[255] = {0};
    char replayFile[255] = {0};
    int size = -1;
    int bFlag = 0;
    int rFlag = 0;
    while ((opt = getopt(argc, argv, options)) != -1) { 
        switch (opt) { 
            case 'o': strncpy(outputFile, optarg, 255); break;
            case 'r': rFlag = 1; strncpy(replayFile, optarg, 255); break;
            case 'b': bFlag = 1; size = atoi(optarg); break;
            default: {
                printf("usage: ./gomoku [-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]\n"
                       "       -r and -b conflicts with each other\n");
                exit(ARGUMENT_ERR);
            }
        } 
    } 

    if (strlen(outputFile) > 0 && outputFile[0] == '-') {
        printf("usage: ./gomoku [-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]\n"
                "       -r and -b conflicts with each other\n");
        exit(ARGUMENT_ERR);
    }
    if (strlen(replayFile) > 0 && replayFile[0] == '-') {
        printf("usage: ./gomoku [-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]\n"
                "       -r and -b conflicts with each other\n");
        exit(ARGUMENT_ERR);
    }
    if (bFlag && (size == -1 || size == 0)) {
        printf("usage: ./gomoku [-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]\n"
                "       -r and -b conflicts with each other\n");
        exit(ARGUMENT_ERR);
    }
    if (bFlag && rFlag) {
        printf("usage: ./gomoku [-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]\n"
                "       -r and -b conflicts with each other\n");
        exit(ARGUMENT_ERR);
    }

    for(; optind < argc; optind++) {      
        printf("usage: ./gomoku [-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]\n"
                "       -r and -b conflicts with each other\n");
        exit(ARGUMENT_ERR);
    }

    game *g = NULL;
    if (replayFile[0] != 0) {
        g = game_import(replayFile);
        if (g->type != GAME_FREESTYLE) {
            exit(RESUME_ERR);
        }
        game_resume(g);
    } else {
        if (size == -1) {
            g = game_create(DEFAULT_SIZE, GAME_FREESTYLE);
        } else {
            g = game_create(size, GAME_FREESTYLE);
        }
        game_loop(g);
    }
    if (outputFile[0] != 0) {
        game_export(g, outputFile);
    }
}