UNAME = $(shell uname -s)
CC = gcc

LEX = flex
YACC = bison
YFLAGS +=--defines=src/y.tab.h -o y.tab.c

CFLAGS = -std=c11 -g -Wall
CFLAGS += -Isrc/
CFLAGS += -D_POSIX_C_SOURCE=200809L
LDFLAGS = -g

ifeq ($(UNAME), Linux)
	LDFLAGS += -ldl -lpthread
endif

OUT=flush

SRC = $(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c)
OBJ = $(SRC:.c=.o)
OBJ += src/parser.o src/scanner.o 

BIN = bin

.POSIX: all clean
.PHONY: all clean

all: dirs src/y.tab.h src/scanner.c compile

dirs: 
	mkdir -p ./$(BIN)

run: all
	$(BIN)/$(OUT)

valgrind: all
	valgrind\
	--track-origins=yes\
	--leak-check=full\
	--leak-resolution=high\
	--show-leak-kinds=all $(MAKE) run

compile: $(OBJ)
	$(CC) -o $(BIN)/$(OUT) $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

src/y.tab.h: src/parser.c
src/scanner.c: src/y.tab.h src/scanner.l

clean:
	rm -rf $(BIN) $(OBJ) src/parser.c src/scanner.c src/*tab.*
