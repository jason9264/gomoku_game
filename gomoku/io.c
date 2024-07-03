/** 
 * @file io.c
 * @author Jason Wang
 * This program controls the input / output for the game. (Read / Write from file)
*/
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error-codes.h"
#include "board.h"
#include "game.h"

/**
 * Saves the move to the games move history
 * @param g the game structure pointer
 * @param x the x coordinate
 * @param y the y coordinate
*/
static void saveMove(game *g, unsigned char x, unsigned char y) {
    if (g->moves_count == g->moves_capacity) {
        g->moves_capacity *= 2;
        g->moves = (move *) realloc(g->moves, g->moves_capacity * sizeof(move));
    }
    move newMove = {x, y, g->stone};
    g->moves[(g->moves_count)++] = newMove;
}

/**
 * Imports a saved game from a file
 * @param path the path to the saved game file
 * @return a pointer to the imported game structure
*/
game* game_import(const char* path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        exit(FILE_INPUT_ERR);
    }
    char mn1, mn2;
    if (fscanf(f, "%c%c", &mn1, &mn2) != 2) {
        exit(FILE_INPUT_ERR);
    }
    if (!(mn1 == 'G' && mn2 == 'A')) {
        exit(FILE_INPUT_ERR);
    }
    int boardSize = 0;
    if (fscanf(f, "%d", &boardSize) != 1) {
        exit(FILE_INPUT_ERR);
    }
    if (boardSize != 15 && boardSize != 17 && boardSize != 19) {
        exit(FILE_INPUT_ERR);
    }
    int gameType = 0;
    if (fscanf(f, "%d", &gameType) != 1) {
        exit(FILE_INPUT_ERR);
    }
    if (gameType < 0 || gameType > 1) {
        exit(FILE_INPUT_ERR);
    }
    int gameState = 0;
    if (fscanf(f, "%d", &gameState) != 1) {
        exit(FILE_INPUT_ERR);
    }
    if (gameState < 0 || gameState > 3) {
        exit(FILE_INPUT_ERR);
    }
    int gameWinner = 0;
    if (fscanf(f, "%d", &gameWinner) != 1) {
        exit(FILE_INPUT_ERR);
    }
    if (gameWinner < 0 || gameWinner > 2) {
        exit(FILE_INPUT_ERR);
    }
    game *g = game_create(boardSize, gameType);
    g->state = gameState;
    g->winner = gameWinner;

    char buffer[50];
    while (fgets(buffer, 50, f)) {
        if (strcmp(buffer, "\n") == 0) {
            continue;
        }
        char line[50] = {0};
        strncpy(line, buffer, 50);
        line[strlen(line) - 1] = 0;
        unsigned char x, y;
        if (board_coord(g->board, line, &x, &y) == FORMAL_COORDINATE_ERR) {
            game_delete(g);
            exit(FILE_INPUT_ERR);
        }
        saveMove(g, x, y);
        board_set(g->board, x, y, g->stone);
        g->stone = (g->stone == BLACK_STONE ? WHITE_STONE : BLACK_STONE);
    }
    fclose(f);
    return g;
}

/**
 * Exports the current game state to a file
 * @param g the game structure pointer
 * @param path the path to save the output file
*/
void game_export(game* g, const char* path) {
    FILE *f = fopen(path, "w");
    if (!f) {
        exit(FILE_OUTPUT_ERR);
    }
    fprintf(f, "GA\n");
    fprintf(f, "%u\n", g->board->size);
    fprintf(f, "%u\n", g->type);
    fprintf(f, "%u\n", g->state);
    fprintf(f, "%u\n", g->winner);
    for (int i = 0; i < g->moves_count; i++) {
        char formalCoord[10] = {0};
        board_formal_coord(g->board, g->moves[i].x, g->moves[i].y, formalCoord);
        fprintf(f, "%s\n", formalCoord);
    }
    fclose(f);
}