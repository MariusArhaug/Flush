UNAME = $(shell uname -s)
CC = gcc

LEX = flex
YACC = bison
YFLAGS +=--defines=src/parser/y.tab.h -o y.tab.c

CFLAGS = -std=c11 -g -Wall
CFLAGS += -Isrc/ -Iinclude/
CFLAGS += -D_XOPEN_SOURCE=600 -D_POSIX_C_SOURCE=200112L
CFLAGS += -DYYSTYPE="void *"
CFLAGS += -Ilib/libgit2/include
# LDFLAGS = -Llib/libgit2 -llibgit2 lib/libgit2/libgit2.so

ifeq ($(UNAME), Linux)
	LDFLAGS += -ldl -lpthread
endif

OUT=flush

SRC = $(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c)
OBJ = $(SRC:.c=.o)
OBJ += src/parser/parser.o src/parser/scanner.o 

BIN = bin

.POSIX: all clean
.PHONY: all clean

all: dirs src/parser/y.tab.h src/parser/scanner.c compile

dirs: 
	mkdir -p ./$(BIN)

libs:
	cd lib/libgit2 && cmake . && cmake --build .

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

src/parser/y.tab.h: src/parser/parser.c
src/parser/scanner.c: src/parser/y.tab.h src/parser/scanner.l

clean:
	- rm -rf $(BIN) $(OBJ) 
	- rm -f src/parser/parser.c src/parser/scanner.c src/parser/*tab.*
