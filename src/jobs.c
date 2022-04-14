#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "jobs.h"

struct job {
  	pid_t pid;
	char* command;
	char* args;
  	struct job* next;
	size_t i;
};

typedef struct job job_t;

void print_jobs(job_t* head) {
	if (head == NULL)
		return;

	printf("    PID	  	      TIME CMD\n");
  	for (job_t* curr = head; curr != NULL; curr = curr->next) {
    	printf("%7d		  %8s %s\n", curr->pid, "00:00:00", curr->command);
  	}
}

job_t* job_init(pid_t pid, const char* command, char* args, size_t i) {
	job_t* job = malloc(sizeof(job_t));
	job->pid = pid;
	job->next = NULL;
	job->command = strdup(command);
	job->args = strdup(args);
	job->i = i;
	return job;
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
  	for (job_t* curr = head; curr != NULL; curr = curr->next) {
    	if (curr->pid == pid) 
    		return curr;
  	}
  	return NULL;
}

void job_destroy(job_t* self) {
	free(self->command);
	free(self->args);
	free(self);
	self = NULL;
}

void delete_job(job_t** head, job_t* del) {

	job_t* temp = *head, *prev;

	if (temp != NULL && temp == del) {
		(*head)->next = temp->next;
		free(temp); // destroy
		return;
	}

	while (temp != NULL && temp != del) {
		prev = temp;
		temp = temp->next;
	}

	if (temp == NULL)
		return;

	prev->next = temp->next;

	free(temp);
}


void cleanup_jobs(job_t** head, size_t* counter) {
	if (*head == NULL || *counter == 0)
		return;

	int status;	
	for (job_t* curr = *head; curr != NULL; curr = curr->next) {

		if (waitpid(curr->pid, &status, WNOHANG) > 0 && WIFEXITED(status)) {
			printf("\n[%ld] + %d done %s %s\n", curr->i, curr->pid, curr->command, curr->args);
			printf("EXIT status [/bin/%s] = %d\n", curr->command, status);

			delete_job(head, curr);
			(*counter)--;
		}	
	}
}