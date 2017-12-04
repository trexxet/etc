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
"exp"             { return T_EXP; }
"ln"              { return T_LN; }
"sin"             { return T_SIN; }
"cos"             { return T_COS; }
"tan"             { return T_TAN; }
"tg"              { return T_TAN; }
"cot"             { return T_COT; }
"ctg"             { return T_COT; }
"arcsin"          { return T_ARCSIN; }
"arccos"          { return T_ARCCOS; }
"arctan"          { return T_ARCTAN; }
"arctg"           { return T_ARCTAN; }
"arccot"          { return T_ARCCOT; }
"arcctg"          { return T_ARCCOT; }
"("               { return T_LPAR; }
")"               { return T_RPAR; }
<<EOF>>           { return T_EOF; }

%%

