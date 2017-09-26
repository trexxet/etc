CC = gcc
CFLAGS = -g -Wall -std=c11
SRC = main.c onegin.c
BIN = onegin

all:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

run: all
	./$(BIN) source.txt out.txt

debug: all
	gdb -q $(BIN)

