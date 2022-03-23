#ifndef STATE_H
#define STATE_H

// Token definitions and other things from bison.
#include "y.tab.h"

/* Generated from bison grammer, call flex specification */
int yyerror(const char* error);

extern int yylineno;
extern int yylex( void );
extern char yytext[];
extern int yylex_destroy( void );

typedef enum {
  COMMAND
} input_t;

#endif