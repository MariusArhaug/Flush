#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "jobs.h"

struct job {
  	pid_t pid;
	const char* command;
  	struct job* next;
};

typedef struct job job_t;

void print_jobs(job_t* head) {
	if (head == NULL)
		return;

	printf("    PID	  	      TIME CMD\n");
  	for (job_t* current = head; current != NULL; current = head->next) {
    	printf("%7d		  %8s %s\n", current->pid, "00:00:00",current->command);
  	}
}

job_t* job_init(pid_t pid, const char* command) {
	job_t* node = malloc(sizeof(job_t));
	node->pid = pid;
	node->next = NULL;
	node->command = command;
	return node;
}

void insert_head(job_t** head, job_t* job) {
  	job->next = *head;
  	*head = job;
}

void insert_after_job(job_t* curr, job_t* job) {
	job->next = curr->next;
	curr->next = job;
}

job_t* find_job(job_t* head, pid_t pid) {
  	for (job_t* curr = head; curr != NULL; curr = head->next) {
    	if (curr->pid == pid) 
    		return curr;
  	}
  	return NULL;
}


void cleanup_jobs(job_t* head) {
	if (head == NULL)
		return;

	int status;
	for (job_t* curr = head; curr != NULL; curr = head->next) {
		if (waitpid(curr->pid, &status, WNOHANG) > 0 && WIFEXITED(status)) {
			printf("EXIT status [/bin/%s] = %d\n", curr->command, status);

			if (curr->next != NULL) {
				for (job_t* n = head; n->next != curr; n = head->next) {
					n->next = curr->next;
				}
				
			}
			// move pointer to next in linkedlist then free it.
			free(curr);
		}	
	}
}