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
#define isSame(_node1, _node2) (_node1 && _node2 && ftree_sameNodes(_node1, _node2))

#define addFunc(_op, _f1, _f2) ftree_addFunction(_op, _f1, _f2)
#define addNum(_n) ftree_addNumber(_n)
#define copy(_f) ftree_copyNode(_f)

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
		replaceFunc (addFunc ("+", copy (Rchild), copy (Lchild)));
	// n - f -> -f + n
	if (OP("-") && isNum(Lchild) && !isNum(Rchild))
		replaceFunc (addFunc ("+",
		                      addFunc ("-", NULL, copy (Rchild)),
		                      copy (Lchild)));
	// f * n -> n * f
	// (f1 / f2) * f3 -> f3 * (f1 / f2)
	if ((OP("*") && isNum(Rchild) && !isNum(Lchild)) ||
	    (OP("*") && isFuncOp(Lchild, "/")))
		replaceFunc (addFunc ("*", copy (Rchild), copy (Lchild)));


	/* Basic arithmetic reduction */
	
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
	// n1 + n2 -> (n1 + n2)
	if (OP("+") && isNum(Lchild) && isNum(Rchild))
		replaceNum (Lchild->num + Rchild->num);
	// n1 - n2 -> (n1 - n2)
	if (OP("-") && isNum(Lchild) && isNum(Rchild))
		replaceNum (Lchild->num - Rchild->num);
	// n1 * n2 -> (n1 * n2)
	if (OP("*") && isNum(Lchild) && isNum(Rchild))
		replaceNum (Lchild->num * Rchild->num);
	// n1 / n2 -> (n1 / n2)
	if (OP("/") && isNum(Lchild) && isNum(Rchild))
		replaceNum (Lchild->num / Rchild->num);
	// n1 ^ n2 -> (n1 ^ n2)
	if (OP("^") && isNum(Lchild) && isNum(Rchild))
		replaceNum (pow(Lchild->num, Rchild->num));


	/* Associative arithmetic reduction */
	
	// (f + n1) + n2 -> f + (n1 + n2)
	if (OP("+") && isFuncOp(Lchild, "+") && isNum(Rchild) && isNum(LRchild))
		replaceFunc (addFunc ("+", 
		                      copy (LLchild), 
		                      addNum (LRchild->num + Rchild->num)));
	// (f - n) - n -> f - (n1 + n2)
	if (OP("-") && isFuncOp(Lchild, "-") && isNum(Rchild) && isNum(LRchild))
		replaceFunc (addFunc ("-", 
		                      copy (LLchild), 
		                      addNum (LRchild->num + Rchild->num)));
	// (f + n1) - n2 -> f + (n1 - n2)
	if (OP("-") && isFuncOp(Lchild, "+") && isNum(Rchild) && isNum(LRchild)) {
		if (LRchild->num >= Rchild->num)
			replaceFunc (addFunc ("+", 
			                      copy (LLchild), 
			                      addNum (LRchild->num - Rchild->num)));
		if (LRchild->num < Rchild->num)
			replaceFunc (addFunc ("-", 
			                      copy (LLchild), 
			                      addNum (Rchild->num - LRchild->num)));
	}
	// (f - n1) + n2 -> f + (n2 - n1)
	if (OP("+") && isFuncOp(Lchild, "-") && isNum(Rchild) && isNum(LRchild)) {
		if (LRchild->num >= Rchild->num)
			replaceFunc (addFunc ("-", 
			                      copy (LLchild), 
			                      addNum (LRchild->num - Rchild->num)));
		if (LRchild->num < Rchild->num)
			replaceFunc (addFunc ("+", 
			                      copy (LLchild), 
			                      addNum (Rchild->num - LRchild->num)));
	}
	// n1 * (n2 * f) -> (n1 * n2) * f
	if (OP("*") && isFuncOp(Rchild, "*") && isNum(Lchild) && isNum(RLchild))
		replaceFunc (addFunc ("*", 
		                      addNum (RLchild->num * Lchild->num),
		                      copy (RRchild)));


	/* Distributive reduction */

	// f + f -> 2 * f
	if (OP("+") && isSame(Lchild, Rchild))
		replaceFunc (addFunc ("*", addNum (2), copy (Lchild)));
	// (n * f) + f -> (n + 1) * f
	if (OP("+") && isFuncOp(Lchild, "*") && isNum(LLchild) && isSame(LRchild, Rchild))
		replaceFunc (addFunc ("*", addNum (LLchild->num + 1), copy (Rchild))); 
	// f + (n * f) -> (n + 1) * f
	if (OP("+") && isFuncOp(Rchild, "*") && isNum(RLchild) && isSame(RRchild, Lchild))
		replaceFunc (addFunc ("*", addNum (RLchild->num + 1), copy (Lchild))); 
	// (n1 * f) + (n2 * f) -> (n1 + n2) * f
	if (OP("+") && isFuncOp(Lchild, "*") && isFuncOp(Rchild, "*") &&
	    isNum(LLchild) && isNum(RLchild) && isSame(LRchild, RRchild))
		replaceFunc (addFunc ("*", addNum (LLchild->num + RLchild->num), copy (LRchild))); 
	// f * f -> f ^ 2
	if (OP("*") && isSame(Lchild, Rchild))
		replaceFunc (addFunc ("^", copy (Lchild), addNum (2)));
	// (f ^ n) * f -> f ^ (n + 1)
	if (OP("*") && isFuncOp(Lchild, "^") && isNum(LRchild) && isSame(LLchild, Rchild))
		replaceFunc (addFunc ("^", copy (Rchild), addNum (LRchild->num + 1)));
	// f * (f ^ n) -> f ^ (n + 1)
	if (OP("*") && isFuncOp(Rchild, "^") && isNum(RRchild) && isSame(RLchild, Lchild))
		replaceFunc (addFunc ("^", copy (Lchild), addNum (RRchild->num + 1)));
	// (f ^ n1) * (f ^ n2) -> f ^ (n1 + n2)
	if (OP("*") && isFuncOp(Lchild, "^") && isFuncOp(Rchild, "^") &&
	    isNum(LRchild) && isNum(RRchild) && isSame(LLchild, RLchild))
		replaceFunc (addFunc ("^", copy (LLchild), addNum (LRchild->num + RRchild->num)));


	/* Associative function reduction */
	
	// f1 * (f2 / f3) -> (f1 * f2) / f3
	if (OP("*") && isFuncOp(Rchild, "/"))
		replaceFunc (addFunc ("/",
		                      addFunc ("*", copy (Lchild), copy (RLchild)),
		                      copy (RRchild)));


	/* Recursively continue reduction */

	int reduceLeft = 0, reduceRight = 0;
	if (Lchild)
		reduceLeft = reduce (&(Lchild));
	if (Rchild)
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
#undef isSame
#undef addNum
#undef addFunc
#undef copy
#undef replaceFunc
#undef replaceNum

