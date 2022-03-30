#ifndef JOBS_H
#define JOBS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct job job_t;

void print_jobs(job_t* head);
job_t* job_init(pid_t pid, const char* command);

void insert_head(job_t** head, job_t* job);
void insert_after_job(job_t* curr, job_t* job);

job_t* find_job(job_t* head, pid_t pid);

void cleanup_jobs(job_t* head);
#endif