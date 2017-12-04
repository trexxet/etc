#include "differentiate.h"
#include <assert.h>
#include <string.h>

#define OP(_op) (strcmp(node->op, _op) == 0)

ftree_node* differentiate (ftree_node *node) {
	assert (node && "Trying to differentiate a NULL node");

	if (node->type == NUMBER)
		return ftree_addNumber (0);
	if (node->type == VARIABLE)
		return ftree_addNumber (1);
	assert (node->type == FUNCTION && "Something has gone wrong while differentiating");

	// Literally differentiate function
	if OP("+")
		return ftree_addFunction ("+", 
					  differentiate (node->lchild),
					  differentiate (node->rchild));
	if OP("-") 
		return ftree_addFunction ("-", 
					  (node->lchild) ? differentiate (node->lchild) : NULL,
					  differentiate (node->rchild));
	if OP("*")
		return ftree_addFunction ("+", 
					  ftree_addFunction ("*",
				                             differentiate (node->lchild),
							     ftree_copyNode (node->rchild)),
					  ftree_addFunction ("*",
				 	                     ftree_copyNode (node->lchild),
							     differentiate (node->rchild)));
	if OP("/")
		return ftree_addFunction ("/",
				          ftree_addFunction ("-",
						             ftree_addFunction ("*",
								                differentiate (node->lchild),
									        ftree_copyNode (node->rchild)),
							     ftree_addFunction ("*",
								                ftree_copyNode (node->lchild),
									        differentiate (node->rchild))),
					  ftree_addFunction ("^",
						             ftree_copyNode (node->rchild),
							     ftree_addNumber (2)));
}

#undef OP

