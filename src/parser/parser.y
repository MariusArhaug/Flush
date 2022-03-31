%{
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <common.h>
#include <string.h>

#include "state.h"

#define STR_EQ(str, pattern) strcmp(str, pattern) == 0

#define strdupn(_s) ((_s) == NULL ? NULL : strdup(_s))

#define ARR(...) \
  (void*[CMD_SIZE]) {__VA_ARGS__}

// resolve path from yytext
static char* handle_path();

//exit 
static void exit_shell();

//push command with args, io redir and bg/curr flag
static void push(void**);

%}

%token CD JOBS PARAM NEWLINE EXIT

%%

input:
    command_list NEWLINE { execute_c(); return 0; } 
  | %empty  { exit_shell(); }
  ;

// TODO rewrite grammer to not support internal commands for pipeing and also not support & along with pipeing.
command_list:
    command_statement                   { push($1); }  
  | command_list '|' command_statement  { push($3); } 
  ;

command_statement: 
    internal_command
  | external_command
  ;

internal_command:
    cd
  | jobs
  | exit
  ;

external_command:
    command param bg                      { $$ = ARR($1, $2, NULL, NULL, $3); }
  | command param '>' param bg            { $$ = ARR($1, $2, NULL,   $4, $5); }
  | command param '<' param bg            { $$ = ARR($1, $2, $4  , NULL, $5); }
  | command param '<' param '>' param bg  { $$ = ARR($1, $2, $4  ,   $6, $7); }
  ;

bg:
    '&'    {$$ = BG;}
  | %empty {$$ = WT;}
  ;

command:
    PARAM { $$ = strdup(yytext); }
  ;

cd: 
    CD path_statment { change_dir($2); }
  ;

path_statment:
    PARAM   { $$ = handle_path();    }
  | %empty  { $$ = strdup(getenv("HOME")); }
  ;

param:
    PARAM   { $$ = strdup(yytext); }
  | %empty  { $$ = NULL; } 
  ;

jobs:
    JOBS { show_jobs(); }
  ;

exit:
    EXIT { exit_shell(); }
  ;

%%

int yyerror( const char* error ) {
  fprintf(stderr, "flush: parse error near \"%s\"\n", yytext);
  return 1;
}

char* handle_path() {
    if (STR_EQ(yytext, "~"))
        return strdup(getenv("HOME"));
    if (STR_EQ(yytext, "-")) {
        printf("%s\n", state.prev_directory);
        return strdup(state.prev_directory);
    }
    return strdup(yytext);
}

void exit_shell() {
    printf("\nflush: shell closed \n"); 
    exit(EXIT_SUCCESS);
}

#define dynamic_arr state.processes

void push(void** arr) {
    if (dynamic_arr->count == dynamic_arr->capacity) {
      dynamic_arr->capacity *= 2;

      dynamic_arr->array = realloc(
        dynamic_arr->array, 
        sizeof(char*) * dynamic_arr->capacity 
      );
    }

	for (int i = 0; i < CMD_SIZE-1; i++) {
		dynamic_arr->array[dynamic_arr->count+i] = strdupn(arr[i]); // malloc and copy 
		free(arr[i]);
	}
	dynamic_arr->array[dynamic_arr->count+CMD_SIZE-1] = arr[CMD_SIZE-1];
 	dynamic_arr->count += CMD_SIZE;
}