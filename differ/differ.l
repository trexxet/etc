%{
#include <stdio.h>
#include <stdlib.h>
#include "functree.h"
#include "differ.tab.h"
%}

%%

[ \t\n]           ;
[0-9]+(\.[0-9]+)? { yylval.numval = strtod(yytext, NULL); return T_NUM; }
"x"               { return T_VAR; }
"+"               { return T_ADD; }
"-"               { return T_SUB; }
"*"               { return T_MUL; }
"/"               { return T_DIV; }
"^"               { return T_POW; }
"("               { return T_LPAR; }
")"               { return T_RPAR; }
<<EOF>>           { return T_EOF; }

%%

