#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define CYN   	"\x1B[36m"
#define RESET   "\x1B[0m"
#define BLD_ON  "\033[1m"
#define BLD_OFF "\033[22m"

int main(int argc, char* argv[]) {
	char cwd[2048];
	while(true) {
		getcwd(cwd, sizeof(cwd));
		printf(BLD_ON CYN "%s: " RESET BLD_OFF, cwd);
		char* line = NULL;
		size_t size;
		getline(&line, &size, stdin);
	}
}
