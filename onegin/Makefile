CC = gcc
CFLAGS = -g -Wall -std=c11
SRC = main.c onegin.c
BIN = onegin


all:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

debug: all
	gdb -q $(BIN)


RUN_FILES = source.txt out.txt

run-s: all
	./$(BIN) s $(RUN_FILES)

run-r: all
	./$(BIN) r $(RUN_FILES)

