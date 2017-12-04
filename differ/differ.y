%{
#include <stdio.h>
#include <stdlib.h>
#include "functree.h"
#include "differentiate.h"
#include "reduce.h"

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
%token T_VAR
%token <nodeval> T_ADD T_SUB T_MUL T_DIV T_POW
%token <nodeval> T_EXP T_LN
%token <nodeval> T_SIN T_COS T_TAN T_COT
%token <nodeval> T_ARCSIN T_ARCCOS T_ARCTAN T_ARCCOT
%token T_LPAR T_RPAR
%token T_EOF

%left T_ADD T_SUB
%left T_MUL T_DIV
%precedence NEG
%right T_POW
%precedence MATHFUNC

%type <nodeval> func

%start launch_EVA01


%%

launch_EVA01: func T_EOF 
	    { 
	        char *str = ftree_str ($1);
		printf ("Function parsed to tree: \n\t%s\n", str);
	        free (str);

		ftree_node *diffed = differentiate ($1);
		str = ftree_str (diffed);
		printf ("Differentiated non-reduced function: \n\t%s\n", str);
		free (str);
		
		printf ("Reducing steps:\n");
		while (reduce (&diffed)) {
			str = ftree_str (diffed);
			printf ("\t%s\n", str);
			free (str);
		}

		ftree_deleteNode (diffed);
	        ftree_deleteNode ($1);
		exit(0);
	    };

func: T_NUM                                      { $$ = ftree_addNumber($1); }
    | T_VAR                                      { $$ = ftree_addVariable(); }
    | func T_ADD func                            { $$ = ftree_addFunction ("+", $1, $3); }
    | func T_SUB func                            { $$ = ftree_addFunction ("-", $1, $3); }
    | func T_MUL func                            { $$ = ftree_addFunction ("*", $1, $3); }
    | func T_DIV func                            { $$ = ftree_addFunction ("/", $1, $3); }
    | T_SUB func %prec NEG                       { $$ = ftree_addFunction ("-", NULL, $2); }
    | func T_POW func                            { $$ = ftree_addFunction ("^", $1, $3); }
    | T_EXP T_LPAR func T_RPAR %prec MATHFUNC    { $$ = ftree_addFunction ("exp", NULL, $3); }
    | T_LN T_LPAR func T_RPAR %prec MATHFUNC     { $$ = ftree_addFunction ("ln", NULL, $3); }
    | T_SIN T_LPAR func T_RPAR %prec MATHFUNC    { $$ = ftree_addFunction ("sin", NULL, $3); }
    | T_COS T_LPAR func T_RPAR %prec MATHFUNC    { $$ = ftree_addFunction ("cos", NULL, $3); }
    | T_TAN T_LPAR func T_RPAR %prec MATHFUNC    { $$ = ftree_addFunction ("tan", NULL, $3); }
    | T_COT T_LPAR func T_RPAR %prec MATHFUNC    { $$ = ftree_addFunction ("cot", NULL, $3); }
    | T_ARCSIN T_LPAR func T_RPAR %prec MATHFUNC { $$ = ftree_addFunction ("arcsin", NULL, $3); }
    | T_ARCCOS T_LPAR func T_RPAR %prec MATHFUNC { $$ = ftree_addFunction ("arccos", NULL, $3); }
    | T_ARCTAN T_LPAR func T_RPAR %prec MATHFUNC { $$ = ftree_addFunction ("arctan", NULL, $3); }
    | T_ARCCOT T_LPAR func T_RPAR %prec MATHFUNC { $$ = ftree_addFunction ("arccot", NULL, $3); }
    | T_LPAR func T_RPAR                         { $$ = $2; }
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

