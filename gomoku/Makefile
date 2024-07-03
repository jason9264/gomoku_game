CC = gcc
CFLAGS = -Wall -std=c99 -g
OBJECTS = io.o board.o game.o

.PHONY: all clean

# Default target
all: gomoku renju replay

# Rule to create gomoku
gomoku: $(OBJECTS) gomoku.o
	$(CC) $(CFLAGS) $^ -o $@

# Rule to create renju
renju: $(OBJECTS) renju.o
	$(CC) $(CFLAGS) $^ -o $@

# Rule to create replay
replay: $(OBJECTS) replay.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean .o files
clean:
	rm -f *.o gomoku renju replay
