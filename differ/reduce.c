#include "reduce.h"
#include <assert.h>
#include <string.h>
#include <math.h>

#define OP(_op) (strcmp(node->op, _op) == 0)
#define Lchild node->lchild
#define Rchild node->rchild
#define LLchild Lchild->lchild
#define LRchild Lchild->rchild
#define RLchild Rchild->lchild
#define RRchild Rchild->rchild

#define isNum(_child) (_child && _child->type == NUMBER)
#define isNumVal(_child, _num) (isNum(_child) && _child->num == _num)
#define isFunc(_child) (_child && _child->type == FUNCTION)
#define isFuncOp(_child, _op) (isFunc(_child) && strcmp(_child->op, _op) == 0)

#define replaceNum(_num) { ftree_replaceNodeNumber (node, _num); return 1; }
#define replaceFunc(_node) { ftree_replaceNodeFunction (pnode, _node); return 1; }


int reduce (ftree_node **pnode) {
	assert (pnode && *pnode && "Trying to reduce a NULL node");
	ftree_node *node = *pnode;

	if (node->type != FUNCTION)
		return 0;


	/* Swap commutative operands */

	// n + f -> f + n
	if (OP("+") && isNum(Lchild) && !isNum(Rchild))
		replaceFunc (ftree_addFunction ("+", ftree_copyNode (Rchild), ftree_copyNode (Lchild)));
	// n - f -> -f + n
	if (OP("-") && isNum(Lchild) && !isNum(Rchild))
		replaceFunc (ftree_addFunction ("+", 
		                                ftree_addFunction("-", NULL,
		                                                  ftree_copyNode(Rchild)), 
		                                ftree_copyNode (Lchild)));
	// f * n -> n * f
	// (f / f) * f -> f * (f / f)
	if ((OP("*") && isNum(Rchild) && !isNum(Lchild)) ||
	    (OP("*") && isFuncOp(Lchild, "/")))
		replaceFunc (ftree_addFunction ("*", ftree_copyNode (Rchild), ftree_copyNode (Lchild)));


	/* Basic arithmetic reducing */
	
	// 0 * f -> 0
	if (OP("*") && (isNumVal(Lchild, 0))) 
		replaceNum (0);
	// 1 * f -> f
	if (OP("*") && isNumVal(Lchild, 1))
		replaceFunc (Rchild); 
	// f + 0 -> f
	// f - 0 -> f
	// f ^ 1 -> f
	if (((OP("+") || OP("-")) && isNumVal(Rchild, 0)) ||
	    (OP("^") && isNumVal(Rchild, 1)))
		replaceFunc (Lchild);
	// n + n -> n
	if (OP("+") && isNum(Lchild) && isNum(Rchild))
		replaceNum (Lchild->num + Rchild->num);
	// n - n -> n
	if (OP("-") && isNum(Lchild) && isNum(Rchild))
		replaceNum (Lchild->num - Rchild->num);
	// n * n -> n
	if (OP("*") && isNum(Lchild) && isNum(Rchild))
		replaceNum (Lchild->num * Rchild->num);
	// n / n -> n
	if (OP("/") && isNum(Lchild) && isNum(Rchild))
		replaceNum (Lchild->num / Rchild->num);
	// n ^ n -> n
	if (OP("^") && isNum(Lchild) && isNum(Rchild))
		replaceNum (pow(Lchild->num, Rchild->num));


	/* Advanced arithmetic reducing */
	
	// (f + n) + n -> f + n
	if (OP("+") && isFuncOp(Lchild, "+") && isNum(Rchild) && isNum(LRchild))
		replaceFunc (ftree_addFunction ("+", 
		                                ftree_copyNode (LLchild), 
		                                ftree_addNumber (LRchild->num + Rchild->num)));
	// (f - n) - n -> f + n
	if (OP("-") && isFuncOp(Lchild, "-") && isNum(Rchild) && isNum(LRchild))
		replaceFunc (ftree_addFunction ("-", 
		                                ftree_copyNode (LLchild), 
		                                ftree_addNumber (LRchild->num + Rchild->num)));
	// (f + n) - n -> f + n
	// (f + n) - n -> f - n
	if (OP("-") && isFuncOp(Lchild, "+") && isNum(Rchild) && isNum(LRchild)) {
		if (LRchild->num >= Rchild->num)
			replaceFunc (ftree_addFunction ("+", 
			                                ftree_copyNode (LLchild), 
			                                ftree_addNumber (LRchild->num - Rchild->num)));
		if (LRchild->num < Rchild->num)
			replaceFunc (ftree_addFunction ("-", 
			                                ftree_copyNode (LLchild), 
			                                ftree_addNumber (Rchild->num - LRchild->num)));
	}
	// (f - n) + n -> f + n
	// (f - n) + n -> f - n
	if (OP("+") && isFuncOp(Lchild, "-") && isNum(Rchild) && isNum(LRchild)) {
		if (LRchild->num >= Rchild->num)
			replaceFunc (ftree_addFunction ("-", 
			                                ftree_copyNode (LLchild), 
			                                ftree_addNumber (LRchild->num - Rchild->num)));
		if (LRchild->num < Rchild->num)
			replaceFunc (ftree_addFunction ("+", 
			                                ftree_copyNode (LLchild), 
			                                ftree_addNumber (Rchild->num - LRchild->num)));
	}
	// n * (n * f) -> n * f
	if (OP("*") && isFuncOp(Rchild, "*") && isNum(Lchild) && isNum(RLchild))
		replaceFunc (ftree_addFunction ("*", 
		                                ftree_addNumber (RLchild->num * Lchild->num),
		                                ftree_copyNode (RRchild)));
	// f * (f / f) -> (f * f) / f
	if (OP("*") && isFuncOp(Rchild, "/"))
		replaceFunc (ftree_addFunction ("/",
		                                ftree_addFunction ("*",
		                                                   ftree_copyNode (Lchild),
		                                                   ftree_copyNode (RLchild)),
		                                ftree_copyNode (RRchild)));


	/* Recursive reduce */

	int reduceLeft = 0, reduceRight = 0;
	if (node->lchild)
		reduceLeft = reduce (&(Lchild));
	if (node->rchild)
		reduceRight = reduce (&(Rchild));
	return reduceLeft | reduceRight;
}


#undef OP
#undef Lchild
#undef Rchild
#undef LLchild
#undef LRchild
#undef RLchild
#undef RRchild
#undef isNum
#undef isNumVal
#undef isFunc
#undef isFuncOp
#undef replaceFunc
#undef replaceNum

