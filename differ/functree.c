#include "functree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


static ftree_node* newNode () {
	ftree_node *node = (ftree_node *) calloc (1, sizeof(ftree_node));
	assert (node && "Can't create node!");
	
	node->lchild = NULL;
	node->rchild = NULL;

	return node;
}


ftree_node* ftree_addNumber (double value) {
	ftree_node *node = newNode ();
	node->type = NUMBER;
	node->num = value;
	return node;
}


ftree_node* ftree_addFunction (char op, ftree_node *lchild, ftree_node *rchild) {
	ftree_node *node = newNode ();
	node->type = FUNCTION;
	node->op = op;
	node->lchild = lchild;
	node->rchild = rchild;
	return node;
}


ftree_node* ftree_addVariable () {
	ftree_node *node = newNode ();
	node->type = VARIABLE;
	return node;
}


void ftree_deleteNode (ftree_node *node) {
	assert (node && "Trying to delete a NULL node");
	if (node->type == FUNCTION) {
		if (node->lchild)
			ftree_deleteNode (node->lchild);
		if (node->rchild)
			ftree_deleteNode (node->rchild);
	}
	free (node);
}


char* ftree_str (ftree_node *node) {
	assert (node && "Trying to print a NULL node");
	char *str = NULL;
	char *strLChild = NULL, *strRChild = NULL;
	switch (node->type) {
		case NUMBER:
			asprintf (&str, "%g", node->num);
			return str;
		case FUNCTION:
			if (node->lchild)
				strLChild = ftree_str (node->lchild);
			else
				strLChild = "";
			if (node->rchild)
				strRChild = ftree_str (node->rchild);
			else
				strRChild = "";
			assert (strLChild && strRChild);
			asprintf (&str, "(%s)%c(%s)", strLChild, node->op, strRChild);
			if (node->lchild && node->lchild->type != VARIABLE)
				free (strLChild);
			if (node->rchild && node->rchild->type != VARIABLE)
				free (strRChild);
			return str;
		case VARIABLE:
			return "x";
		default:
			assert (0 && "Something has gone wrong while building string");
	}
}

