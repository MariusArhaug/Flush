UNAME = $(shell uname -s)
CC = gcc

CFLAGS = -std=c11 -g -Wall
CFLAGS += -Isrc/ -Ilib/curl/include
CFLAGS += -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700
LDFLAGS = -g

ifeq ($(UNAME), Linux)
	LDFLAGS += -ldl -lpthread
endif

OUT=mtwwwd

ENV = .env

WEB_PATH = $(shell grep "WEB_PATH" $(ENV) | cut -d '=' -f2)
PORT = $(shell grep "PORT" $(ENV) | cut -d '=' -f2)
N_THREADS = $(shell grep "N_THREADS" $(ENV) | cut -d '=' -f2)
N_BUFFS = $(shell grep "N_BUFFS" $(ENV) | cut -d '=' -f2)

SRC = $(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c)
OBJ = $(SRC:.c=.o)

BIN = bin

.POSIX: all clean
.PHONY: all clean

all: dirs compile

dirs: 
	mkdir -p ./$(BIN)

run: all
	$(BIN)/$(OUT) $(WEB_PATH) $(PORT) $(N_THREADS) $(N_BUFFS)

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
