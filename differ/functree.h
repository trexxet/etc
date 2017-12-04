#pragma once
#define _GNU_SOURCE


typedef struct _ftree_node {
	enum {
		NUMBER,
		FUNCTION,
		VARIABLE
	} type;
	union {
		// Number data
		double num;
		// Function data
		struct {
			char op;
			struct _ftree_node *lchild;
			struct _ftree_node *rchild;
		};
	};
} ftree_node;


ftree_node* ftree_addNumber (double value);
ftree_node* ftree_addFunction (char op, ftree_node *lchild, ftree_node *rchild);
ftree_node* ftree_addVariable ();
void ftree_deleteNode (ftree_node *node);
char* ftree_str (ftree_node *node);

