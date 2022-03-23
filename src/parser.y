%{
#include <stdlib.h>
#include <stdio.h>
#include "state.h"
%}

%token CMD

%%

command: 
  CMD { printf("text %s \n", yytext); printf("%d \n", $$); }
  ;


%%

int yyerror( const char* error ) {
  fprintf(stderr, "%s on line %d\n", error, yylineno);
  exit(EXIT_FAILURE);
}