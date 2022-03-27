%{
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <common.h>
#include "state.h"
%}

%token COMMAND CD JOBS PARAM PATH NEWLINE

%%

input:
    command_list NEWLINE { return NULL; }
  ;

command_list:
    command_statement                   
  | command_list '|' command_statement
  ;

command_statement: 
    internal_command
  | external_command
  ;

internal_command:
    cd
  | jobs
  ;

//https://stackoverflow.com/questions/14543443 in-c-how-do-you-redirect-stdin-stdout-stderr-to-files-when-making-an-execvp-or
external_command:
    command                      { fork_and_execute($1, stdin, stdout, NULL); }
  | command '&'                  { /* backgoround process */ }
  | command '<' param            { /* io redirection */} 
  | command '<' param '>' param  {}
  ;

command:
    COMMAND { $$ = strdup(yytext); }
  ;

param:
    PARAM
  ;

cd: 
    CD path_statment { change_dir($2); }
  ;

path_statment:
    path
  | %empty  { $$ = strdup(getenv("HOME")); }
  ;

path:
    '~'        { $$ = strdup(getenv("HOME")); }
  | '-'        { $$ = strdup(state.prev_directory); }
  | CD PATH    { $$ = strdup(yytext); }
  ;


jobs:
  JOBS { show_jobs(); }
  ;

%%

int yyerror( const char* error ) {
  fprintf(stderr, "%s on line %d, yytext: \"%s\"\n", error, yylineno, yytext);
  exit(EXIT_FAILURE); 
}