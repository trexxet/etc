#include "differentiate.h"
#include <assert.h>
#include <string.h>

#define OP(_op) (strcmp(node->op, _op) == 0)
#define newfunc(_op, _f1, _f2) return ftree_addFunction(_op, _f1, _f2)
#define addfunc(_op, _f1, _f2) ftree_addFunction(_op, _f1, _f2)
#define cpfunc(_f) ftree_copyNode(_f)
#define Lchild node->lchild
#define Rchild node->rchild

ftree_node* differentiate (ftree_node *node) {
	assert (node && "Trying to differentiate a NULL node");

	if (node->type == NUMBER)
		return ftree_addNumber (0);
	if (node->type == VARIABLE)
		return ftree_addNumber (1);
	assert (node->type == FUNCTION && "Something has gone wrong while differentiating");

	// Literally differentiate function
	if OP("+")
		newfunc ("+", 
		         differentiate (Lchild),
		         differentiate (Rchild));
	if OP("-") 
		newfunc ("-", 
		         (Lchild) ? differentiate (Lchild) : NULL,
		         differentiate (Rchild));
	if OP("*")
		newfunc ("+", 
	                 addfunc ("*",
		                  differentiate (Lchild),
		                  cpfunc (Rchild)),
		         addfunc ("*",
		                  cpfunc (Lchild),
		                  differentiate (Rchild)));
	if OP("/")
		newfunc ("/",
		         addfunc ("-",
		                  addfunc ("*",
		                           differentiate (Lchild),
		                           cpfunc (Rchild)),
		                  addfunc ("*",
		                           cpfunc (Lchild),
		                           differentiate (Rchild))),
		         addfunc ("^",
		                  cpfunc (Rchild),
		                  ftree_addNumber (2)));
	if OP("^") {
		if (Lchild->type == VARIABLE && Rchild->type == NUMBER)
			newfunc ("*",
			         ftree_addNumber (Rchild->num),
			         addfunc ("^",
			                  ftree_addVariable (),
			                  ftree_addNumber (Rchild->num - 1)));
		newfunc ("*",
		         cpfunc (node),
		         differentiate (addfunc ("*",
		                                 cpfunc (Rchild),
		                                 addfunc ("ln", NULL, 
		                                          cpfunc(Lchild)))));
	}
	if OP("exp")
		newfunc ("*",
		         cpfunc (node),
		         differentiate (Rchild));
	if OP("ln")
		newfunc ("/",
		         differentiate (Rchild),
		         cpfunc (Rchild));
	if OP("sin")
		newfunc ("*",
		         addfunc ("cos", NULL,
		                  cpfunc (Rchild)),
		         differentiate (Rchild));
	if OP("cos")
		newfunc ("*",
		         addfunc ("-", NULL,
		                  addfunc ("sin", NULL,
		                           cpfunc (Rchild))),
		         differentiate (Rchild));
	if OP("tan")
		newfunc ("/",
		         differentiate (Rchild),
		         addfunc ("^",
		                  addfunc ("cos", NULL, 
		                           cpfunc (Rchild)),
		                  ftree_addNumber (2)));
	if OP("cot")
		newfunc ("-", NULL,
		         addfunc ("/",
		                  differentiate (Rchild),
		                  addfunc ("^",
		                           addfunc ("sin", NULL,
		                                    cpfunc (Rchild)),
		                           ftree_addNumber (2))));
	if OP("arcsin")
		newfunc ("/",
		         differentiate (Rchild),
			 addfunc ("^",
		                  addfunc ("-",
		                           ftree_addNumber(1),
		                           addfunc ("^",
		                                    cpfunc (Rchild),
		                                    ftree_addNumber(2))),
		                  ftree_addNumber(0.5)));
	if OP("arccos")
		newfunc ("-", NULL,
		         addfunc ("/",
		                  differentiate (Rchild),
			          addfunc ("^",
		                           addfunc ("-",
		                                    ftree_addNumber(1),
		                                    addfunc ("^",
		                                             cpfunc (Rchild),
		                                             ftree_addNumber(2))),
		                           ftree_addNumber(0.5))));
	if OP("arctan")
		newfunc ("/",
		         differentiate (Rchild),
		         addfunc ("+",
		                  ftree_addNumber (1),
		                  addfunc ("^",
		                           cpfunc (Rchild),
		                           ftree_addNumber (2))));
	if OP("arccot")
		newfunc ("-", NULL,
		         addfunc ("/",
		                  differentiate (Rchild),
		                  addfunc ("+",
		                           ftree_addNumber (1),
		                           addfunc ("^",
		                                    cpfunc (Rchild),
		                                    ftree_addNumber (2)))));
}

#undef OP
#undef newfunc
#undef addfunc
#undef cpfunc
#undef Lchild
#undef Rchild

