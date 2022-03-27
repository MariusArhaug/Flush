#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct state {
  struct job* jobs;  
  bool prev_success;
  char prev_directory[2048];
};

struct job {
  pid_t pid;
};

extern struct state state; //global state

void fork_and_execute(const char* command, FILE* in, FILE* out, const char* args);

void change_dir(const char* path);
void show_jobs(void);

#endif