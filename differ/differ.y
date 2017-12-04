%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functree.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;

void yyerror (const char *msg);
%}

%union {
	ftree_node* nodeval;
	double numval;
}

%token <numval> T_NUM
%token <nodeval> T_ADD T_SUB T_MUL T_DIV T_POW
%token T_VAR
%token T_LPAR T_RPAR
%token T_EOF

%left T_ADD T_SUB
%left T_MUL T_DIV
%precedence NEG
%right T_POW

%type <nodeval> func

%start launch_EVA01


%%

launch_EVA01: func T_EOF 
	    { 
	        char *str = ftree_str ($1);
		printf ("%s\n", str);
	        free (str);
	        ftree_deleteNode ($1);
		exit(0);
	    };

func: T_NUM { $$ = ftree_addNumber($1); }
    | T_VAR { $$ = ftree_addVariable(); }
    | func T_ADD func { $$ = ftree_addFunction ('+', $1, $3); }
    | func T_SUB func { $$ = ftree_addFunction ('-', $1, $3); }
    | func T_MUL func { $$ = ftree_addFunction ('*', $1, $3); }
    | func T_DIV func { $$ = ftree_addFunction ('/', $1, $3); }
    | T_SUB func %prec NEG { $$ = ftree_addFunction ('-', ftree_addNumber(0), $2); }
    | func T_POW func { $$ = ftree_addFunction ('^', $1, $3); }
    | T_LPAR func T_RPAR { $$ = $2; }
    ;

%%


int main (int argc, char *argv[]) {
	yyin = fopen (argv[1], "r");
	if (!yyin) {
		fprintf (stderr, "Cannot open input file\n");
		return 1;
	}
	do {
		yyparse();
	} while (!feof(yyin));
}

void yyerror (const char * msg) {
	fprintf (stderr, "Failure: %s\n", msg);
	exit(1);
}

