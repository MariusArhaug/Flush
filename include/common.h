#ifndef COMMON_H__
#define COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

// Token definitions and other things from bison.
#include "parser/y.tab.h"

/* Generated from bison grammer, call flex specification */
int yyerror(const char* error);

extern int yylineno;
extern int yylex( void );
extern char yytext[];
extern int yylex_destroy( void );

#endif