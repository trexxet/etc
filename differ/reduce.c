#include "reduce.h"
#include <assert.h>
#include <string.h>
#include <math.h>

#define OP(_op) (strcmp(node->op, _op) == 0)

int reduce (ftree_node **pnode) {
	assert (pnode && *pnode && "Trying to reduce a NULL node");
	ftree_node *node = *pnode;

	if (node->type != FUNCTION)
		return 0;

	// 0 * f -> 0
	// f * 0 -> 0
	if (OP("*") && ((node->lchild->type == NUMBER && node->lchild->num == 0) ||
	                (node->rchild->type == NUMBER && node->rchild->num == 0))) {
		ftree_replaceNodeNumber (node, 0); return 1;
	}
	// 0 + f -> f
	if (OP("+") && node->lchild->type == NUMBER && node->lchild->num == 0) {
		ftree_replaceNodeFunction (pnode, node->rchild); return 1;
	}
	// f + 0 -> f
	if (OP("+") && node->rchild->type == NUMBER && node->rchild->num == 0) {
		ftree_replaceNodeFunction (pnode, node->lchild); return 1;
	}
	// f - 0 -> f
	if (OP("-") && node->rchild->type == NUMBER && node->rchild->num == 0) {
		ftree_replaceNodeFunction (pnode, node->lchild); return 1;
	}
	// 0 - f -> -f
	if (OP("-") && node->lchild->type == NUMBER && node->lchild->num == 0) {
		ftree_replaceNodeFunction (pnode, ftree_addFunction("-", NULL, node->rchild)); return 1;
	}
	// 1 * f -> f
	if (OP("*") && node->lchild->type == NUMBER && node->lchild->num == 1) {
		ftree_replaceNodeFunction (pnode, node->rchild); return 1;
	}
	// f * 1 -> f
	if (OP("*") && node->rchild->type == NUMBER && node->rchild->num == 1) {
		ftree_replaceNodeFunction (pnode, node->lchild); return 1;
	}
	// n + n -> n
	if (OP("+") && node->lchild->type == NUMBER && node->rchild->type == NUMBER) {
		ftree_replaceNodeNumber (node, node->lchild->num + node->rchild->num); return 1;
	}
	// n - n -> n
	if (OP("-") && node->lchild && node->lchild->type == NUMBER && node->rchild->type == NUMBER) {
		ftree_replaceNodeNumber (node, node->lchild->num - node->rchild->num); return 1;
	}
	// n * n -> n
	if (OP("*") && node->lchild->type == NUMBER && node->rchild->type == NUMBER) {
		ftree_replaceNodeNumber (node, node->lchild->num * node->rchild->num); return 1;
	}
	// n / n -> n
	if (OP("/") && node->lchild->type == NUMBER && node->rchild->type == NUMBER) {
		ftree_replaceNodeNumber (node, node->lchild->num / node->rchild->num); return 1;
	}
	// n ^ n -> n
	if (OP("^") && node->lchild->type == NUMBER && node->rchild->type == NUMBER) {
		ftree_replaceNodeNumber (node, pow(node->lchild->num, node->rchild->num)); return 1;
	}

	int reduceLeft = 0, reduceRight = 0;
	if (node->lchild)
		reduceLeft = reduce (&(node->lchild));
	if (node->rchild)
		reduceRight = reduce (&(node->rchild));
	return reduceLeft | reduceRight;
}

#undef OP

