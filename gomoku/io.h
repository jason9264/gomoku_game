#ifndef _IO_H_
#define _IO_H_
#include "game.h"

/** Function to import a game*/
game* game_import(const char* path);
// void game_export(game* g, bool binary, const char* path);
/** Function to export a game*/
void game_export(game* g, const char* path);
#endif