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
PARAM [~\-\"a-zA-Z0-9\/\.\']+
CD cd
JOBS jobs
EXIT exit

%%

{WHITESPACE}            { /* eliminate whitespace */}
{NEWLINE}               { return NEWLINE;   }
{CD}                    { return CD;        }
{JOBS}                  { return JOBS;      }
{EXIT}                  { return EXIT;      }
{PARAM}                 { return PARAM;     }
.                       { return yytext[0]; }
%%