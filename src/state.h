#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "jobs.h"

#define CYN   	"\x1B[36m"
#define GRN 	"\x1B[32m"
#define RED 	"\x1B[31m"
#define RESET   "\x1B[0m"
#define BLD_ON  "\033[1m"
#define BLD_OFF "\033[22m"

struct state {
    bool prev_success;
    char prev_directory[2048];
    job_t* head;
    size_t job_count;
    struct process_array* processes;
};

// size of cmd array
#define CMD_SIZE 5

// original capacity of dynamic cmd array, needs to be multiple of cmd_size
#define INIT_CAP (CMD_SIZE * 3)
struct process_array {
    void** array;
	int capacity;
	int count;
};

extern struct state state; //global state

#define BG (void*) 1
#define WT (void*) 2
typedef void* flag_t;


//execute stored programs to be executed
void execute_c(void);

// change dir to given path, prints to stderr if path is not found
void change_dir(const char* path);

// show all background jobs still running
void show_jobs(void);

#endif