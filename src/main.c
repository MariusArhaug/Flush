#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termio.h>
#include <fcntl.h>

#include <git2.h>
#include <common.h>

#include "state.h"
#include "jobs.h"

#define PERROR(_err, ...) do {		\
	perror(_err, ##__VA_ARGS__); 	\
	exit(EXIT_FAILURE); 			\
} while(0)

#define PRINT_ARROW() printf("%s%s  " RESET, state.prev_success ? GRN : RED, "\u2794")

#define PRINT_CWD()												\
	char _cwd[2048];											\
	getcwd(_cwd, sizeof(_cwd));									\
	printf(BLD_ON CYN "%s: " RESET BLD_OFF, _cwd)

#define IN  1
#define OUT 0

// execute and wait or run in backgroud depending in on flag
static void execute(const char* command, char* args, char* input, char* output, flag_t flag);
static void exec(const char* command, char* args, char* input, char*output);
static void cleanup( void );

struct state state; 

void change_dir(const char* path) {
	char cwd[2048];
	getcwd(cwd, sizeof(cwd));
	if (chdir(path) == -1) {
		fprintf(stderr, "cd: no such file or directory: %s \n", path);
		state.prev_success = false;
		return;
	}
	strcpy(state.prev_directory, cwd);
	state.prev_success = true;
}

void execute_c(void) {

	// if only one push has been done we know its a single execution, no pipes
	if (state.processes->count == CMD_SIZE) {
		return execute(
			state.processes->array[0], // cmd
			state.processes->array[1], // arg
			state.processes->array[2], // in
			state.processes->array[3], // out
			state.processes->array[4]  // bg / wt
		);
	}
	
	// always a multiple of 5, need n - 1 pipes for n calls
	int n_cmds = (state.processes->count / CMD_SIZE);
	int n_pipes = n_cmds - 1;
	int fd[n_pipes * 2]; 

	for (int i = 0; i < n_pipes; i++)
		if (pipe(fd + i*2) < 0)
			PERROR("pipe init");

	int status;
	int j = 0;
	for (int i = 0; i < n_cmds; i++) {
		pid_t pid = fork();
		if (pid < 0)
			PERROR("pid");

		if (pid == 0) {
	
			//if not first cmd
			if (i != 0) {
				if (dup2(fd[(j-1)*2], 0) < 0)
					PERROR("dup2 0");
			}

			// if not last cmd
			if (i != n_cmds -1) {
				if (dup2(fd[j*2+1], 1) < 0)
					PERROR("dup2 1");	
			}		

			for (int k = 0; k < n_pipes*2; k++) 
				close(fd[k]);

			
			exec(
				state.processes->array[(i*CMD_SIZE)+0], // cmd
				state.processes->array[(i*CMD_SIZE)+1], // arg
				state.processes->array[(i*CMD_SIZE)+2], // in
				state.processes->array[(i*CMD_SIZE)+3]  // out
			);
		}
		
		j++;
	}

	for (int i = 0; i < n_pipes*2; i++)
		close(fd[i]);

	for (int i = 0; i < n_cmds; i++) {
		wait(&status); 
		printf("EXIT status [/bin/%s] = %d\n", state.processes->array[(i*CMD_SIZE)+0], status);
	}	
}

void exec(const char* command, char* args, char* input, char* output) {
	char bin[1024] = "/bin/";
	strcat(bin, command);

	if (access(bin, X_OK) != 0) {
		fprintf(stderr, "flush: command not found: \"%s\"\n", bin);
  		state.prev_success = false;
		return;
	}

	signal(SIGINT, SIG_DFL); // allow signal for current process
	if (input != NULL) {
		int fd = open(input, O_RDONLY | O_CREAT, 0);
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	if (output != NULL) {
		int fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}

	char* const execv_args[3] = {
		bin,
		args,
		NULL
	};
	if (execv(execv_args[0], execv_args) == -1)
			exit(errno);
	exit(EXIT_SUCCESS);
}

void execute(const char* command, char* args, char* input, char* output, flag_t flag) {
	pid_t pid = fork();
	
	if (pid == 0) {
		exec(command, args, input, output);
	}

	if (flag == WT) {
		int status;
		waitpid(pid, &status, 0);
		state.prev_success = status == EXIT_SUCCESS;
		printf("EXIT status [/bin/%s] = %d\n", command, status);
	} 

	if (flag == BG) {
		job_t* job = job_init(pid, command);
		if (state.job_count == 0) 
			state.head = job;
		else 
			insert_after_job(state.head, job);
		
		state.job_count++;
		printf("[%ld] %d %s %s\n", state.job_count, pid, command, args == NULL ? "" : args);
	}
}

void processes_init() {
	state.processes->capacity = INIT_CAP;
	state.processes->count = 0;
	state.processes->array = malloc(sizeof(void*) * state.processes->capacity);
	memset(state.processes->array, 0, sizeof(void*) * state.processes->capacity);
}

void init() {
	if (fork() == 0) {					
		char* const args[3] = {
			"/bin/stty",
			"-echoctl",
			NULL,
		};
		execv("/bin/stty", args); //remove ^C from stdout
		exit(EXIT_SUCCESS);
	}

	signal(SIGINT, SIG_IGN); // disable ctrl + c from main shell only child proesses
	state.prev_success = true;
	getcwd(state.prev_directory, sizeof(state.prev_directory));
	state.job_count = 0;
	state.head = NULL;

	state.processes = malloc(sizeof(struct process_array));
	processes_init();
	
}

void show_jobs( void ) {
	print_jobs(state.head);
}

void cleanup( void ) {

	for (int i = 0; i < state.processes->count / CMD_SIZE; i++) {
		for (int j = 0 ; j < 4; j++) {
			if (state.processes->array[(i*CMD_SIZE)+j] != NULL)
				free(state.processes->array[(i*CMD_SIZE)+j]);
		}
	}

	free(state.processes->array);
	processes_init();
	//cleanup_jobs(state.head);
}

void prompt( void ) {
	PRINT_ARROW();
	PRINT_CWD();

	yyparse();
	cleanup();
}

int main(int argc, char* argv[]) {
	init();
	while(true) {
		prompt();
	}
}