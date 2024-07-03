/** 
 * @file board.c
 * @author Jason Wang
 * This program will provide the following functions to manage game structs, implements the rule sets, and interacts with players
*/

#include "game.h"
#include "error-codes.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
/**
 * Saves a move in the game structure
 * @param g the Game structure pointer
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
 * Checks if the game is finished by determing if there is a winning alignment
 * @param g the game struct pointer
 * @return code 1 if game is finished and there is a winner code 0 otherwise.
*/
static int isGameFinished(game *g) {
    unsigned char size = g->board->size;
    unsigned char* grid = g->board->grid;
    unsigned char i, j;

    // Check horizontal alignment
    for (i = 0; i < size; i++) {
        for (j = 0; j < size - 4; j++) {
            unsigned char stone = grid[i * size + j];
            if (stone != EMPTY_INTERSECTION && 
                stone == grid[i * size + j + 1] && 
                stone == grid[i * size + j + 2] && 
                stone == grid[i * size + j + 3] && 
                stone == grid[i * size + j + 4]) {
                return 1;
            }
        }
    }

    // Check vertical alignment
    for (i = 0; i < size - 4; i++) {
        for (j = 0; j < size; j++) {
            unsigned char stone = grid[i * size + j];
            if (stone != EMPTY_INTERSECTION && 
                stone == grid[(i + 1) * size + j] && 
                stone == grid[(i + 2) * size + j] && 
                stone == grid[(i + 3) * size + j] && 
                stone == grid[(i + 4) * size + j]) {
                return 1;
            }
        }
    }

    // Check diagonal alignment (top-left to bottom-right)
    for (i = 0; i < size - 4; i++) {
        for (j = 0; j < size - 4; j++) {
            unsigned char stone = grid[i * size + j];
            if (stone != EMPTY_INTERSECTION && 
                stone == grid[(i + 1) * size + (j + 1)] && 
                stone == grid[(i + 2) * size + (j + 2)] && 
                stone == grid[(i + 3) * size + (j + 3)] && 
                stone == grid[(i + 4) * size + (j + 4)]) {
                return 1;
            }
        }
    }

    // Check diagonal alignment (top-right to bottom-left)
    for (i = 0; i < size - 4; i++) {
        for (j = 4; j < size; j++) {
            unsigned char stone = grid[i * size + j];
            if (stone != EMPTY_INTERSECTION && 
                stone == grid[(i + 1) * size + (j - 1)] && 
                stone == grid[(i + 2) * size + (j - 2)] && 
                stone == grid[(i + 3) * size + (j - 3)] && 
                stone == grid[(i + 4) * size + (j - 4)]) {
                return 1;
            }
        }
    }

    return 0; // Game is not finished
}

/**
 * counts the number of consecutive stones in a line on the gird
 * @param grid the grid
 * @param size the size of the grid
 * @param x the starting x coord
 * @param y the starting y coord
 * @param dx the x direction
 * @param dy the y direction
 * @param stone the Stone value
 * @return the number of consecutive stones in line
*/
static int countLine(const unsigned char* grid, unsigned char size, int x, int y, int dx, int dy, unsigned char stone) {
    int count = 0;
    int i, j;

    for (i = x + dx, j = y + dy; i >= 0 && i < size && j >= 0 && j < size; i += dx, j += dy) {
        if (grid[i * size + j] == stone) {
            count++;
        } else {
            break;
        }
    }

    return count;
}

/**
 * Checks if there is a winning four stones on the grid
 * @param grid the game grid
 * @param x the x coordinates
 * @param y the y coordinate
 * @param stone the stone value to check
 * @return code 1 if there is a winner return code 0 otherwise
*/
static int isFour(const unsigned char* grid, unsigned char size, int x, int y, unsigned char stone) {
    int count = 0;

    // Check horizontal
    count += countLine(grid, size, x, y, 0, 1, stone) + countLine(grid, size, x, y, 0, -1, stone);
    //printf("Count after horizontal = %d\n", count);
    if (count == 4) {
        return 1;
    }
    count = 0;
    // Check vertical
    count += countLine(grid, size, x, y, 1, 0, stone) + countLine(grid, size, x, y, -1, 0, stone);
    //printf("Count after vertical = %d\n", count);
    if (count == 4) {
        return 1;
    }
    count = 0;
    // Check diagonal (top-left to bottom-right)
    count += countLine(grid, size, x, y, 1, 1, stone) + countLine(grid, size, x, y, -1, -1, stone);
    //printf("Count after TL-BR = %d\n", count);
    if (count == 4) {
        return 1;
    }
    count = 0;
    // Check diagonal (top-right to bottom-left)
    count += countLine(grid, size, x, y, 1, -1, stone) + countLine(grid, size, x, y, -1, 1, stone);
    //printf("Count after TR-BL = %d\n", count);
    if (count == 4) {
        return 1;
    }
    return 0;
}

/**
 * Checks if there is an overline which is a winning alignment of more than 5 stones
 * @param grid the game grid
 * @param size the size of the game grid
 * @param x the x coordinates
 * @param y the y coordinates
 * @param stone the stone value
 * @return code 1 if there is a winner code 0 otherwise
*/
static int isOverline(const unsigned char* grid, unsigned char size, int x, int y, unsigned char stone) {
    // Check horizontal
    if (countLine(grid, size, x, y, 0, 1, stone) + countLine(grid, size, x, y, 0, -1, stone) >= 5) {
        //printf("Horizontal\n");
        return 1;
    }

    // Check vertical
    if (countLine(grid, size, x, y, 1, 0, stone) + countLine(grid, size, x, y, -1, 0, stone) >= 5) {
        //printf("Vertical\n");
        return 1;
    }

    // Check diagonal (top-left to bottom-right)
    if (countLine(grid, size, x, y, 1, 1, stone) + countLine(grid, size, x, y, -1, -1, stone) >= 5) {
        //printf("TL-BR\n");
        return 1;
    }

    // Check diagonal (top-right to bottom-left)
    if (countLine(grid, size, x, y, 1, -1, stone) + countLine(grid, size, x, y, -1, 1, stone) >= 5) {
        //printf("TR-BL\n");
        return 1;
    }

    return 0;
}

/**
 * Checks if the move is forbidden in the game based on coordinates
 * @param g the game struct pointer
 * @param x the x coordinate
 * @param y the y coordinate
 * @return 1 if the move is forbidden 0 otherwise.
*/
int isMoveForbidden(game *g, int x, int y) {
    //printf("in isMoveForbidden for x=%c, y = %d\n", x, y);
    unsigned char size = g->board->size;
    unsigned char* grid = g->board->grid;
    int xGrid = y - 1;
    int yGrid = x - 'A';

    // Check if the move creates at least two open fours
    if (isFour(grid, size, xGrid, yGrid, BLACK_STONE) >= 2) {
        //printf("isFOur\n");
        return 1;
    }

    // Check if the move creates at least one overline
    if (isOverline(grid, size, xGrid, yGrid, BLACK_STONE)) {
        //printf("Overline\n");
        return 1;
    }

    return 0;
}

/**
 * Checks if the move is forbideen in the game based on coordinates
 * @param g the game struct pointer
 * @param x the x coordinate
 * @param y the y coordinate
 * @return 1 if foribbiden 0 if otherwise.
*/
int isMoveForbidden2(game *g, int x, int y) {
    // check horizontally
    // check same row, left
    int blackStonesToLeft = 0;
    int isEmpyLeftEnd = 0;
    int xL = x - 1;
    while (xL >= 'A') {
        if (board_get(g->board, xL, y) == BLACK_STONE) {
            blackStonesToLeft++;
        } else {
            break;
        }
        xL--;
    }
    if (xL >= 'A') {
        if (board_get(g->board, xL, y) == EMPTY_INTERSECTION) {
            isEmpyLeftEnd = 1;
        }
    }
    // check same row, right
    int blackStonesToRight = 0;
    int isEmpyRightEnd = 0;
    int xR = x + 1;
    while (xL < 'A' + g->board->size) {
        if (board_get(g->board, xR, y) == BLACK_STONE) {
            blackStonesToRight++;
        } else {
            break;
        }
        xR++;
    }
    if (xL < 'A' + g->board->size) {
        if (board_get(g->board, xR, y) == EMPTY_INTERSECTION) {
            isEmpyRightEnd = 1;
        }
    }
    if (1 + blackStonesToLeft + blackStonesToRight == 4 && isEmpyLeftEnd && isEmpyRightEnd) {
        return 1;
    }

    // check vertically

    // check same column, up
    int blackStonesUp = 0;
    int isEmptyUpEnd = 0;
    int yU = y + 1;
    while (yU <= g->board->size) {
        if (board_get(g->board, x, yU) == BLACK_STONE) {
            blackStonesUp++;
        } else {
            break;
        }
        yU++;
    }
    if (yU <= g->board->size) {
        if (board_get(g->board, x, yU) == EMPTY_INTERSECTION) {
            isEmptyUpEnd = 1;
        }
    }

    // check same column, down
    int blackStonesDown = 0;
    int isEmptyDownEnd = 0;
    int yD = y - 1;
    while (yD > 0) {
        if (board_get(g->board, x, yD) == BLACK_STONE) {
            blackStonesDown++;
        } else {
            break;
        }
        yD--;
    }
    if (yD > 0) {
        if (board_get(g->board, x, yD) == EMPTY_INTERSECTION) {
            isEmptyDownEnd = 1;
        }
    }
    if (1 + blackStonesUp + blackStonesDown == 4 && isEmptyUpEnd && isEmptyDownEnd) {
        return 1;
    }

    // check diagonally bottom left to top right
    // decrease row, decrease column
    int blackStonesLeftDown = 0;
    int isEmptyLeftDownEnd = 0;
    int xLD = x - 1;
    int yLD = y - 1;
    while (xLD >= 'A' && yLD > 0) {
        if (board_get(g->board, xLD, yLD) == BLACK_STONE) {
            blackStonesLeftDown++;
        } else {
            break;
        }
        xLD--;
        yLD--;
    }
    if (xLD >= 'A' && yLD > 0) {
        if (board_get(g->board, xLD, yLD) == EMPTY_INTERSECTION) {
            isEmptyLeftDownEnd = 1;
        }
    }
    // increase row, increase column
    int blackStonesRightUp = 0;
    int isEmptyRightUpEnd = 0;
    int xRU = x + 1;
    int yRU = y + 1;
    while (xRU < 'A' + g->board->size && yRU <= g->board->size) {
        if (board_get(g->board, xRU, yRU) == BLACK_STONE) {
            blackStonesRightUp++;
        } else {
            break;
        }
        xRU++;
        yRU++;
    }
    if (xRU < 'A' + g->board->size && yRU <= g->board->size) {
        if (board_get(g->board, xRU, yRU) == EMPTY_INTERSECTION) {
            isEmptyRightUpEnd = 1;
        }
    }
    if (blackStonesLeftDown + blackStonesRightUp + 1 == 4 && isEmptyLeftDownEnd && isEmptyRightUpEnd) {
        return 1;
    }

    // check diagonally bottom right to top left
    // increase row, decrease column
    int blackStonesLeftUp = 0;
    int isEmptyLeftUpEnd = 0;
    int xLU = x - 1;
    int yLU = y + 1;
    while (xLU >= 'A' && yLU <= g->board->size) {
        if (board_get(g->board, xLU, yLU) == BLACK_STONE) {
            blackStonesLeftUp++;
        } else {
            break;
        }
        xLU--;
        yLU++;
    }
    if (xLU >= 'A' && yLU <= g->board->size) {
        if (board_get(g->board, xLU, yLU) == EMPTY_INTERSECTION) {
            isEmptyLeftUpEnd = 1;
        }
    }
    // decrease row, increase column
    int blackStonesRightDown = 0;
    int isEmptyRightDownEnd = 0;
    int xRD = x + 1;
    int yRD = y - 1;
    while (xRD < 'A' + g->board->size && yRD > 0) {
        if (board_get(g->board, xRD, yRD) == BLACK_STONE) {
            blackStonesRightDown++;
        } else {
            break;
        }
        xRD++;
        yRD--;
    }
    if (xRD < 'A' + g->board->size && yRD > 0) {
        if (board_get(g->board, xRD, yRD) == EMPTY_INTERSECTION) {
            isEmptyRightDownEnd = 1;
        }
    }

    if (blackStonesLeftUp + blackStonesRightDown + 1 == 4 && isEmptyLeftUpEnd && isEmptyRightDownEnd) {
        return 1;
    }
    return 0;
}

/**
 * Creates a new game with the specified board size and game type
 * @param board_size the size of the game board
 * @param game_type the type of the game
 * @return A pointer to the new game or null if malloc fails.
*/
game* game_create(unsigned char board_size, unsigned char game_type) {
    game *newGame = (game *) malloc (sizeof(game));
    if (!newGame) {
        return NULL;
    }
    newGame->board = board_create(board_size);
    newGame->type = game_type;
    newGame->stone = BLACK_STONE;
    newGame->state = GAME_STATE_PLAYING;
    newGame->winner = EMPTY_INTERSECTION;
    newGame->moves = (move *) malloc(16 * sizeof(move));
    if (!newGame->moves) {
        board_delete(newGame->board);
        free(newGame);
        return NULL;
    }
    newGame->moves_capacity = 16;
    newGame->moves_count = 0;

    return newGame;
}

/**
 * Deletes the game
 * @param g the pointer to the game
*/
void game_delete(game* g) {
    if (!g) {
        exit(NULL_POINTER_ERR);
    }
    board_delete(g->board);
    free(g->moves);
    free(g);
}

/**
 * Updates the game state by processing moves and completion
 * @param g the game struct pointer
 * @return true if the game is succesfully updated, false otherwise.
*/
bool game_update(game* g) {
    if (g->state != GAME_STATE_PLAYING) {
        return false;
    }
    bool hasUserIntroducedAValidMove = false;
    while (!hasUserIntroducedAValidMove) {
        // Prompt
        if (g->stone == BLACK_STONE) {
            printf("Black stone's turn, please enter a move: ");
        } else {
            printf("White stone's turn, please enter a move: ");
        }
        // Read user input
        char input[50] = {0};
        int pos = 0;
        char inputChar = getchar();
        while (inputChar != EOF && inputChar != '\n') {
            input[pos++] = inputChar;
            inputChar = getchar();
        }
        if (inputChar == EOF) {
            if (strlen(input) > 0) {
                // run one more round
                unsigned char x, y;
                if (!(board_coord(g->board, input, &x, &y) == SUCCESS)) {
                    printf("The coordinate you entered is invalid, please try again.\n");
                    printf("The game is stopped.\n");
                    g->state = GAME_STATE_STOPPED;
                    return false;
                }
                if (board_get(g->board, x, y) != EMPTY_INTERSECTION) {
                    printf("There is already a stone at the coordinate you entered, please try again.\n");
                    printf("The game is stopped.\n");
                    g->state = GAME_STATE_STOPPED;
                    return false;
                }
                saveMove(g, x, y);
                board_set(g->board, x, y, g->stone);
                board_print(g->board, true);
                if (g->type == GAME_RENJU && g->stone == BLACK_STONE) {
                    if (isMoveForbidden(g, x, y)) {
                        printf("Game concluded, black made a forbidden move, white won.\n");
                        g->state = GAME_STATE_FORBIDDEN;
                        g->winner = WHITE_STONE;
                        return false;
                    }
                }
                if (isGameFinished(g)) {
                    char *winnerStr = g->stone == BLACK_STONE ? "black" : "white";
                    printf("Game concluded, %s won.\n", winnerStr);
                    g->state = GAME_STATE_FINISHED;
                    g->winner = g->stone;
                    return false;
                }
                if (board_is_full(g->board)) {
                    printf("Game concluded, the board is full, draw.\n");
                    g->state = GAME_STATE_FINISHED;
                    return true;
                }
                g->stone = (g->stone == BLACK_STONE ? WHITE_STONE : BLACK_STONE);
                if (g->stone == BLACK_STONE) {
                    printf("Black stone's turn, please enter a move: ");
                } else {
                    printf("White stone's turn, please enter a move: ");
                }
            }
            printf("The game is stopped.\n");
            g->state = GAME_STATE_STOPPED;
            return false;
        }
        unsigned char x, y;
        if (!(board_coord(g->board, input, &x, &y) == SUCCESS)) {
            printf("The coordinate you entered is invalid, please try again.\n");
            continue;
        }
        if (game_place_stone(g, x, y)) {
            hasUserIntroducedAValidMove = true;
        }
    }
    return true;
}

/**
 * Runs the game loop continuously updating state and printing the board
 * @param g the game structure pointer
*/
void game_loop(game* g) {
    board_print(g->board, true);
    int gameOn = game_update(g);
    while (gameOn) {
        //printf("gameON\n");
        if (g->winner) {
            break;
        }
        board_print(g->board, true);
        gameOn = game_update(g);
    }
}

/**
 * Resumes the game
 * @param g the game structure pointer
*/
void game_resume(game* g) {
    if (g->state != GAME_STATE_STOPPED) {
        exit(RESUME_ERR);
    }
    g->state = GAME_STATE_PLAYING;
    game_loop(g);
}

/**
 * Replays the game
 * @param g the game structure pointer
*/
void game_replay(game* g) {
    game *ng = game_create(g->board->size, g->type);

    move firstMove = g->moves[0];
    saveMove(ng, firstMove.x, firstMove.y);
    board_set(ng->board, firstMove.x, firstMove.y, ng->stone);
    ng->stone = (ng->stone == BLACK_STONE ? WHITE_STONE : BLACK_STONE);
    board_print(ng->board, true);
    char buffer[50];
    board_formal_coord(ng->board, firstMove.x, firstMove.y, buffer);
    printf("Moves:\n");
    printf("Black: %3s", buffer);
    //sleep(1);
    int finishEarlier = 0;
    for (int i = 1; i < g->moves_count; i++) {
        saveMove(ng, g->moves[i].x, g->moves[i].y);
        board_set(ng->board, g->moves[i].x, g->moves[i].y, ng->stone);
        board_print(ng->board, true);
        if (ng->type == GAME_RENJU && ng->stone == BLACK_STONE) {
            if (isMoveForbidden(ng, ng->moves[i].x, ng->moves[i].y)) {
                printf("Game concluded, black made a forbidden move, white won.\n");
                ng->state = GAME_STATE_FORBIDDEN;
                ng->winner = WHITE_STONE;
                finishEarlier = 1;
            }
        }
        if (!finishEarlier) {
            if (isGameFinished(ng)) {
                char *winnerStr = ng->stone == BLACK_STONE ? "black" : "white";
                printf("Game concluded, %s won.\n", winnerStr);
                ng->state = GAME_STATE_FINISHED;
                ng->winner = g->stone;
                finishEarlier = 1;
            }
        }
        if (!finishEarlier) {
            if (board_is_full(ng->board)) {
                printf("Game concluded, the board is full, draw.\n");
                ng->state = GAME_STATE_FINISHED;
                finishEarlier = 1;
            }
        }
        ng->stone = (ng->stone == BLACK_STONE ? WHITE_STONE : BLACK_STONE);
        if (i == g->moves_count - 1 && !finishEarlier) {
            printf("The game is stopped.\n");
            finishEarlier = 1;
        }
        //sleep(1);
        int lastBlack = 0;
        printf("Moves:\n");
        for (int j = 0; j < ng->moves_count; j++) {
            char buffer[50];
            board_formal_coord(ng->board, ng->moves[j].x, ng->moves[j].y, buffer);
            if (j % 2 == 0) {
                printf("Black: %3s", buffer);
                lastBlack = 1;
            } else {
                printf("  White: %3s\n", buffer);
                lastBlack = 0;
            }
        }
        if (finishEarlier && lastBlack) {
            printf("\n");
        }
    }
    game_delete(ng);  
}

/**
 * Places a stone at the specified location
 * @param g the game structure pointer
 * @param x the x coordinate to place
 * @param y the y coordinate to place
 * @return true if success, false otherwise.
*/
bool game_place_stone(game* g, unsigned char x, unsigned char y) {
    if (board_get(g->board, x, y) != EMPTY_INTERSECTION) {
        printf("There is already a stone at the coordinate you entered, please try again.\n");
        return false;
    }
    saveMove(g, x, y);
    board_set(g->board, x, y, g->stone);
    if (g->type == GAME_RENJU && g->stone == BLACK_STONE) {
        if (isMoveForbidden(g, x, y)) {
            board_print(g->board, true);
            printf("Game concluded, black made a forbidden move, white won.\n");
            g->state = GAME_STATE_FORBIDDEN;
            g->winner = WHITE_STONE;
            return true;
        }
    }
    if (isGameFinished(g)) {
        char *winnerStr = g->stone == BLACK_STONE ? "black" : "white";
        printf("Game concluded, %s won.\n", winnerStr);
        g->state = GAME_STATE_FINISHED;
        g->winner = g->stone;
        return true;
    }
    if (board_is_full(g->board)) {
        printf("Game concluded, the board is full, draw.\n");
        g->state = GAME_STATE_FINISHED;
        return true;
    }
    g->stone = (g->stone == BLACK_STONE ? WHITE_STONE : BLACK_STONE);
    return true;
}