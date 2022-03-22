UNAME = $(shell uname -s)
CC = gcc

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

BIN = bin

.POSIX: all clean
.PHONY: all clean

all: dirs compile

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

clean:
	rm -rf $(BIN) $(OBJ)
