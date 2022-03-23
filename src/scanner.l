%{
#include "state.h"  
%}
%option noyywrap
%option array
%option yylineno

WHITESPACE [\ \t\v\r\n]+
COMMAND [a-zA-Z]([a-zA-z0-9_])*

%%

{WHITESPACE}            { /* eliminate whitespace */}
{COMMAND}               { return COMMAND; }
.                       { return yytext[0]; }
%%