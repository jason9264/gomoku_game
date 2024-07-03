# Gomoku Game

This is the main program to play the Gomoku game, a traditional board game where the objective is to align five stones in a row. The program supports loading and saving game states, as well as customizing the board size.

## Features

	•	Start New Game: Start a new game with a customizable board size (15, 17, or 19).
	•	Load Game: Resume an unfinished match from a saved file.
	•	Save Game: Save the current game state to a file for later continuation.

## Usage

	•	./gomoku [-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]
       -r and -b conflicts with each other

	•	-r <unfinished-match.gmk>: Load an unfinished match from the specified file.
	•	-o <saved-match.gmk>: Save the current match to the specified file.
	•	-b <15|17|19>: Start a new game with a board size of 15, 17, or 19.

## Compilation

To compile the program, run:

	•	gcc -o gomoku board.c game.c io.c -I .

Ensure that you have error-codes.h, board.h, game.h, and io.h in your project directory.

## Example

## Start a new game with a 15x15 board and save the progress:

	•	./gomoku -b 15 -o mygame.gmk

## Load an unfinished game from a file:

	•	./gomoku -r mygame.gmk

## Error Handling

	•	The program ensures that the -r and -b options are not used together.
	•	Validates command-line arguments and displays usage information if invalid arguments are detected.

## Exit Status

The program returns specific exit statuses defined in error-codes.h for different error conditions, such as ARGUMENT_ERR and RESUME_ERR.



