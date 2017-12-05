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
			const char *op;
			struct _ftree_node *lchild;
			struct _ftree_node *rchild;
		};
	};
} ftree_node;


ftree_node* ftree_addNumber (double value);
ftree_node* ftree_addFunction (const char *op, ftree_node *lchild, ftree_node *rchild);
ftree_node* ftree_addVariable ();

ftree_node* ftree_copyNode (ftree_node *node);
void ftree_deleteNode (ftree_node *node);

int ftree_sameNodes (ftree_node *node1, ftree_node *node2);

void ftree_replaceNodeNumber (ftree_node *node, double value);
void ftree_replaceNodeFunction (ftree_node **pnode, ftree_node *newNode);
void ftree_replaceNodeVariable (ftree_node *node);

void ftree_printNode (ftree_node *node);
char* ftree_str (ftree_node *node);

