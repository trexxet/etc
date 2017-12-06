#include "reduce.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#define OP(_op) (strcmp(node->op, _op) == 0)
#define Lchild (node->lchild)
#define Rchild (node->rchild)
#define LLchild (Lchild->lchild)
#define LRchild (Lchild->rchild)
#define RLchild (Rchild->lchild)
#define RRchild (Rchild->rchild)

#define isNum(_child) (_child && _child->type == NUMBER)
#define isNumVal(_child, _num) (isNum(_child) && _child->num == _num)
#define isFunc(_child) (_child && _child->type == FUNCTION)
#define isFuncOp(_child, _op) (isFunc(_child) && strcmp(_child->op, _op) == 0)
#define isSame(_node1, _node2) (_node1 && _node2 && ftree_sameNodes(_node1, _node2))

#define addFunc(_op, _f1, _f2) ftree_addFunction(_op, _f1, _f2)
#define addNum(_n) ftree_addNumber(_n)
#define copy(_f) ftree_copyNode(_f)

#define cpLchild (Lchild ? copy(Lchild) : NULL)
#define cpRchild copy(Rchild)
#define cpLLchild (LLchild ? copy(LLchild) : NULL)
#define cpLRchild copy(LRchild)
#define cpRLchild (RLchild ? copy(RLchild) : NULL)
#define cpRRchild copy(RRchild)

#define replaceNum(_num) { ftree_replaceNodeNumber (node, _num); return 1; }
#define replaceFunc(_op, _l, _r) { ftree_replaceNodeFunction (pnode, addFunc(_op, _l, _r)); return 1; }


int debugging = 0;
int reduceCalls = 0;

static void debug_printNode (ftree_node *node) {
	if (!debugging)
		return;
	char *str = ftree_str (node);
	if (node->type == FUNCTION)
		printf ("#%s\t %s\n", node->op, str);
	else
		printf ("\t %s\n", str);
	free (str);
}

#define dprintf(...) if (debugging) printf(__VA_ARGS__)

static int swapBasicCommutative (ftree_node **pnode);


int reduce (ftree_node **pnode) {
	assert (pnode && *pnode && "Trying to reduce a NULL node");
	reduceCalls++;
	ftree_node *node = *pnode;
	static int reducingSwapped = 0;

	debug_printNode (node);

	if (node->type != FUNCTION)
		return 0;
		
	if (reducingSwapped)
		goto reduceSwapped;


	/* Swap commutative operands */
	dprintf ("Swapping commutative operands...\n");
	ftree_node *saveNode = copy (node);
	while (swapBasicCommutative (pnode));
	node = *pnode;
	dprintf ("Swapping result:");
	if (isSame(node, saveNode))
		dprintf (" Nothing changed lol\n");
	else {
		dprintf ("\n");
		debug_printNode (node);
	}
	ftree_deleteNode (saveNode);


	/* Basic arithmetic reduction */

	// -(0) -> 0
	// 0 * f -> 0
	if ((OP("-") && !Lchild && isNumVal(Rchild, 0)) ||
	    (OP("*") && isNumVal(Lchild, 0)))
		replaceNum (0);
	// 1 * f -> f
	if (OP("*") && isNumVal(Lchild, 1))
		{ ftree_replaceNodeFunction (pnode, Rchild); return 1; }
	// -1 * f -> -f
	if (OP("*") && isNumVal(Lchild, -1))
		replaceFunc ("-", NULL, cpRchild);
	// f + 0 -> f
	// f - 0 -> f
	// f ^ 1 -> f
	if (((OP("+") || OP("-")) && Lchild && isNumVal(Rchild, 0)) ||
	    (OP("^") && isNumVal(Rchild, 1)))
		{ ftree_replaceNodeFunction (pnode, Lchild); return 1; }
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


reduceSwapped:
	/* Associative arithmetic reduction */

	// (f - n1) - n2 -> f - (n1 + n2)
	if (OP("-") && isFuncOp(Lchild, "-") && isNum(Rchild) && isNum(LRchild))
		replaceFunc (node->op, cpLLchild, addNum (LRchild->num + Rchild->num));
	// (f + n1) - n2 -> f + (n1 - n2)
	if (OP("-") && isFuncOp(Lchild, "+") && isNum(Rchild) && isNum(LRchild))
		replaceFunc (Lchild->op, cpLLchild, addNum (LRchild->num - Rchild->num));
	// n1 * (n2 * f) -> (n1 * n2) * f
	if (OP("*") && isFuncOp(Rchild, "*") && isNum(Lchild) && isNum(RLchild))
		replaceFunc ("*", addNum (RLchild->num * Lchild->num), cpRRchild);


	/* Distributive reduction */

	// f + f -> 2 * f
	if (OP("+") && isSame(Lchild, Rchild))
		replaceFunc ("*", addNum (2), cpLchild);
	// (n * f) + f -> (n + 1) * f
	if (OP("+") && isFuncOp(Lchild, "*") && isNum(LLchild) && isSame(LRchild, Rchild))
		replaceFunc ("*", addNum (LLchild->num + 1), cpRchild);
	// (f1 * f2) + (f3 * f2) -> f2 * (f1 + f3)
	if (OP("+") && isFuncOp(Lchild, "*") && isFuncOp(Rchild, "*") && isSame(LRchild, RRchild))
		replaceFunc ("*", cpLRchild, addFunc ("+", cpLLchild, cpRLchild));
	// (f1 * f2) + (f2 * f3) -> f2 * (f1 + f3)
	if (OP("+") && isFuncOp(Lchild, "*") && isFuncOp(Rchild, "*") && isSame(LRchild, RLchild))
		replaceFunc ("*", cpLRchild, addFunc ("+", cpLLchild, cpRRchild));
	// (f2 * f1) + (f3 * f2) -> f2 * (f1 + f3)
	if (OP("+") && isFuncOp(Lchild, "*") && isFuncOp(Rchild, "*") && isSame(LLchild, RRchild))
		replaceFunc ("*", cpLLchild, addFunc ("+", cpLRchild, cpRLchild));
	// (f2 * f1) + (f2 * f3) -> f2 * (f1 + f3)
	if (OP("+") && isFuncOp(Lchild, "*") && isFuncOp(Rchild, "*") && isSame(LLchild, RLchild))
		replaceFunc ("*", cpLLchild, addFunc ("+", cpLRchild, cpRRchild));
	// f * f -> f ^ 2
	if (OP("*") && isSame(Lchild, Rchild))
		replaceFunc ("^", cpLchild, addNum (2));
	// (f ^ n) * f -> f ^ (n + 1)
	if (OP("*") && isFuncOp(Lchild, "^") && isNum(LRchild) && isSame(LLchild, Rchild))
		replaceFunc ("^", cpRchild, addNum (LRchild->num + 1));
	// (f ^ n1) * (f ^ n2) -> f ^ (n1 + n2)
	if (OP("*") && isFuncOp(Lchild, "^") && isFuncOp(Rchild, "^") &&
	    isNum(LRchild) && isNum(RRchild) && isSame(LLchild, RLchild))
		replaceFunc ("^", cpLLchild, addNum (LRchild->num + RRchild->num));


	/* Other function reduction */

	// f1 + -(f2) -> f1 - f2
	if (OP("+") && isFuncOp(Rchild, "-") && !RLchild)
		replaceFunc ("-", cpLchild, cpRRchild);
	// f1 * (f2 / f3) -> (f1 * f2) / f3
	if (OP("*") && isFuncOp(Rchild, "/"))
		replaceFunc ("/", addFunc ("*", cpLchild, cpRLchild), cpRRchild);


	/* Try to reduce associative function */
	// If reducing associative and still nothing has been reduced, then return
	// (f1 + f2) + f3 -> f1 + (f3 + f2)
	// (f1 - f2) + f3 -> f1 + (f3 - f2)
	// (f1 * f2) * f3 -> f1 * (f3 * f2)
	if ((OP("+") && (isFuncOp(Lchild, "+") || isFuncOp(Lchild, "-"))) ||
	    (OP("*") && isFuncOp(Lchild, "*"))) {
		dprintf ("Associating (f1 %s f2) %s f3 -> f1 %s (f3 %s f2) and going right\n",
		         Lchild->op, node->op, node->op, Lchild->op);
		ftree_node *assocNode = addFunc (node->op, cpLLchild, 
						 addFunc(Lchild->op, cpRchild, cpLRchild));
		if (reduce (&assocNode->rchild)) {
			dprintf ("Associative reduced! Result:\n");
			debug_printNode (assocNode);
			ftree_replaceNodeFunction (pnode, assocNode);
			return 1;
		} 
		else {
			dprintf ("Associative wasn't reduced! Returning to\n");
			debug_printNode (node);
			ftree_deleteNode (assocNode);
		}
	}
	// 


	/* Try to reduce commutative function */
	// If reducing commutative and still nothing has been reduced, then return
	if (reducingSwapped) {
		reducingSwapped = 0;
		return 0;
	}
	else 
	// f1 + f2 -> f2 + f1
	// f1 * f2 -> f2 * f1
	if (OP("+") || OP("*")) {
		dprintf ("Swapping f1 %s f2 -> f2 %s f1\n", node->op, node->op);
		reducingSwapped = 1;
		ftree_node *swapNode = addFunc (node->op, cpRchild, cpLchild);
		if (reduce (&swapNode)) {
			dprintf ("Commutative reduced! Result:\n");
			debug_printNode (swapNode);
			reducingSwapped = 0;
			ftree_replaceNodeFunction (pnode, swapNode);
			return 1;
		}
		else {
			dprintf ("Commutative wasn't reduced! Returning to\n");
			debug_printNode (node);
			reducingSwapped = 0;
			ftree_deleteNode (swapNode);
		}
	}


	/* Recursively continue reduction */

	int reducedLeft = 0, reducedRight = 0;
	if (Lchild) {
		dprintf ("Going left\n");
		reducedLeft = reduce (&(Lchild));
	}
	if (Rchild) {
		dprintf ("Going right\n");
		reducedRight = reduce (&(Rchild));
	}
	dprintf ("Going up to\n");
	debug_printNode (*pnode);
	return reducedLeft | reducedRight;
}


/* Swap commutative operands */
static int swapBasicCommutative (ftree_node **pnode) {
	assert (pnode && *pnode && "Trying to swap a NULL node");
	ftree_node *node = *pnode;
	
	if (node->type != FUNCTION)
		return 0;

	// n + f -> f + n
	if (OP("+") && isNum(Lchild) && !isNum(Rchild))
		replaceFunc ("+", cpRchild, cpLchild);
	// n - f -> -f + n
	if (OP("-") && isNum(Lchild) && !isNum(Rchild))
		replaceFunc ("+", addFunc ("-", NULL, cpRchild), cpLchild);
	// f * n -> n * f
	// (f1 / f2) * f3 -> f3 * (f1 / f2)
	if ((OP("*") && isNum(Rchild) && !isNum(Lchild)) ||
	    (OP("*") && isFuncOp(Lchild, "/")))
		replaceFunc ("*", cpRchild, cpLchild);

	/* Recursively continue swaping */

	int swapedLeft = 0, swapedRight = 0;
	if (Lchild)
		swapedLeft = swapBasicCommutative (&(Lchild));
	if (Rchild)
		swapedRight = swapBasicCommutative (&(Rchild));
	return swapedLeft | swapedRight;
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
#undef cpLchild
#undef cpRchild
#undef cpLLchild
#undef cpLRchild
#undef cpRLchild
#undef cpRRchild
#undef replaceFunc
#undef replaceNum

