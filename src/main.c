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

#include <git2.h>
#include <common.h>

#include "state.h"

#define CYN   	"\x1B[36m"
#define GRN 		"\x1B[32m"
#define RED 		"\x1B[31m"
#define RESET   "\x1B[0m"
#define BLD_ON  "\033[1m"
#define BLD_OFF "\033[22m"

struct state state; 

#define PRINT_ARROW()					\
	printf("%s%s  " RESET, state.prev_success ? GRN : RED, "\u2794")

#define PRINT_CWD()									\
	char _cwd[2048];								\
	getcwd(_cwd, sizeof(_cwd));						\
	printf(BLD_ON CYN "%s: " RESET BLD_OFF, _cwd)


void change_dir(const char* path) {
	if (chdir(path) == -1) {
		fprintf(stderr, "cd: no such file or directory: %s \n", path);
		state.prev_success = false;
	}
	state.prev_success = true;
}

static void prompt() {
	PRINT_ARROW();
	PRINT_CWD();

	yyparse();
}



void fork_and_execute(const char* command, FILE* in, FILE* out, const char* args) {

	const char bin[1024] = "/bin/";
	strcat(bin, command);

	if (access(bin, X_OK) != 0) {
		fprintf(stderr, "flush: command not found: \"%s\"\n", command);
  	state.prev_success = false;
		return;
	}

	pid_t pid = fork();
	
	if (pid == 0) {
			dup2(fileno(in), STDIN_FILENO);
			dup2(fileno(out), STDOUT_FILENO);

			char * execv_args[2];
			execv_args[0] = bin;
			execv_args[1] = args;

			if (execv(execv_args[0], execv_args) == -1);
				exit(errno);
			exit(EXIT_SUCCESS);
		
	} else {
		int status;
		waitpid(pid, &status, 0);
		if (status == EXIT_SUCCESS) {
			state.prev_success = true;
		}
		printf("EXIT status [%s] = %d\n", command, status);
	} 
	
}

static void push_job(pid_t pid) {

}

void show_jobs(void) {

}


int main(int argc, char* argv[]) {
	state.prev_success = true;
	getcwd(state.prev_directory, sizeof(state.prev_directory));
	while(true) {
		prompt();
	}
}