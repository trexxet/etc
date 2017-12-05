#include "functree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


static ftree_node* newNode () {
	ftree_node *node = (ftree_node *) calloc (1, sizeof(ftree_node));
	assert (node && "Can't create node!");
	
	node->lchild = node->rchild = NULL;

	return node;
}


ftree_node* ftree_addNumber (double value) {
	ftree_node *node = newNode ();
	node->type = NUMBER;
	node->num = value;
	return node;
}


ftree_node* ftree_addFunction (const char *op, ftree_node *lchild, ftree_node *rchild) {
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


ftree_node* ftree_copyNode (ftree_node *node) {
	assert (node && "Trying to copy a NULL node");

	switch (node->type) {
		case NUMBER:
			return ftree_addNumber (node->num);
		case FUNCTION:
			return ftree_addFunction (node->op,
			                          (node->lchild) ? ftree_copyNode (node->lchild) : NULL,
						  ftree_copyNode (node->rchild));
		case VARIABLE:
			return ftree_addVariable ();
		default:
			assert (0 && "Something has gone wrong while copying node");
	}
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


int ftree_sameNodes (ftree_node *node1, ftree_node *node2) {
	assert (node1 && node2 && "Trying to compare a NULL node");

	if (node1->type != node2->type)
		return 0;

	if (node1->type == NUMBER && node1->num != node2->num)
		return 0;

	if (node1->type == FUNCTION) {
		if (strcmp (node1->op, node2->op) != 0)
			return 0;
		if (node1->lchild && node2->lchild)
			return ftree_sameNodes (node1->lchild, node2->lchild) & 
			       ftree_sameNodes (node1->rchild, node2->rchild);
		else if (!node1->lchild && !node2->lchild)
			return ftree_sameNodes (node1->rchild, node2->rchild);
		else
			return 0;
	}

	return 1;
}


void ftree_replaceNodeNumber (ftree_node *node, double value) {
	assert (node && "Trying to replace a NULL node");

	if (node->lchild)
		ftree_deleteNode (node->lchild);
	if (node->rchild)
		ftree_deleteNode (node->rchild);

	node->lchild = node->rchild = NULL;
	node->type = NUMBER;
	node->num = value;
}


void ftree_replaceNodeFunction (ftree_node **pnode, ftree_node *newNode) {
	assert (pnode && *pnode && "Trying to replace a NULL node");
	assert (newNode && "Trying to replace with a NULL node");
	ftree_node *node = *pnode;

	if (node->lchild == newNode)
		node->lchild = NULL;
	if (node->rchild == newNode)
		node->rchild = NULL;
	ftree_deleteNode (node);
	
	*pnode = newNode;
}


void ftree_replaceNodeVariable (ftree_node *node) {
	assert (node && "Trying to replace a NULL node");

	if (node->lchild)
		ftree_deleteNode (node->lchild);
	if (node->rchild)
		ftree_deleteNode (node->rchild);

	node->lchild = node->rchild = NULL;
	node->type = VARIABLE;
}


void ftree_printNode (ftree_node *node) {
	assert (node && "Trying to print a NULL node");

	fprintf (stderr, "Node %p\n\tType: %d\n", node, node->type);
	if (node->type == NUMBER)
		fprintf (stderr, "\tValue: %g\n", node->num);
	if (node->type == FUNCTION)
		fprintf (stderr, "\tOperator: %s\n\tLChild: %p\n\tRChild: %p\n", 
		         node->op, node->lchild, node->rchild);
}


char* ftree_str (ftree_node *node) {
	assert (node && "Trying to build string from a NULL node");
	char *str = NULL;
	char *strLChild = NULL, *strRChild = NULL;

	switch (node->type) {
		case NUMBER:
			asprintf (&str, "%g", node->num);
			return str;
		case FUNCTION:
			strLChild = (node->lchild) ? ftree_str (node->lchild) : NULL;
			strRChild = (node->rchild) ? ftree_str (node->rchild) : strdup("");
			if (strLChild)
				asprintf (&str, "(%s)%s(%s)", strLChild, node->op, strRChild);
			else
				asprintf (&str, "%s(%s)", node->op, strRChild);
			free (strLChild);
			free (strRChild);
			return str;
		case VARIABLE:
			return strdup("x");
		default:
			assert (0 && "Something has gone wrong while building string");
	}
}

