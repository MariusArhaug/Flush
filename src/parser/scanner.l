%{
#include <common.h>  
%}
%option noyywrap
%option array
%option yylineno
%option noinput
%option nounput

WHITESPACE [\ \t\v\r]+
NEWLINE [\n]
COMMAND [a-zA-Z]([a-zA-Z0-9_])*
PARAM [$!?"a-zA-Z0-9\/\.]+
PATH ^\/([^\/]+)$
CD cd
JOBS jobs

%%

{WHITESPACE}            { /* eliminate whitespace */}
{NEWLINE}               { return NEWLINE;   }
{CD}                    { return CD;        }
{JOBS}                  { return JOBS;      }
{PATH}                  { return PATH;      }
{COMMAND}               { return COMMAND;   }
{PARAM}                 { return PARAM;     }
.                       { return yytext[0]; }
%%