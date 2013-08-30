#ifndef __IR_H__
#define __IR_H__

#include "compiler.h"

typedef struct Operand_ Operand;
typedef struct Operand_ *OperandPtr;
typedef struct InterCode_ InterCode;
typedef struct InterCode_ *InterCodePtr;
typedef struct InterCodeNode_ InterCodeNode;
typedef struct InterCodeNode_ *ICN_Ptr;

typedef enum {
	RELOP_EQ,    /* == equal to */
	RELOP_NE,    /* != not equal */
	RELOP_LT,    /*  < less than */
	RELOP_GT,    /*  > great than */
	RELOP_LE,    /* <= less equal */
	RELOP_GE     /* >= great equal */
} RelopKind;

typedef enum{
	VARIABLE,
	CONSTANT,
	ADDRESS,
	ADDRESS_GET,
	OP_LABEL,
	FUNCTION,
	OP_PARAM,
	TEMP
} OpKind;

typedef enum{
	_ASSIGN,
	_ADD,
	_SUB,
	_MUL,
	_DIV,
	_RELOP,
	IR_LABEL,
	_GOTO,
	_RETURN,
	_CALL,
	_DEC,
	_ARG,
	_READ,
	_WRITE,
	DEF_FUNCTION,
	IR_PARAM,
	VAR_DEC
} IrKind;

struct Operand_ {
	OpKind kind;
	union {
		int var_no;
		int addr_no;
		int value;
		char *label_name;
		char *func_name;
		char *param_name;
		char *id_name;
	} u;
};


struct InterCode_ {
	IrKind kind;
	union {
		struct {
			OperandPtr right, left;
		} assign;

		struct {
			OperandPtr op;
		} uniop;

		struct {
			OperandPtr result, op1, op2;
		} binop;

		struct {
			OperandPtr relop1, relop2, label;
			RelopKind reltype;
		} cond;

	} u;
};

struct InterCodeNode_ {
	InterCodePtr code;
	ICN_Ptr prev, next;
};

int generate_ir(PtrTreeNode);


/* insert an inter code into the inter code table */
void add_inter_code(ICN_Ptr *table, InterCodePtr inter_code);

/* bind an inter code list to another inter code list */
ICN_Ptr bind_inter_code(ICN_Ptr code1, ICN_Ptr code2);

//delete an inter code list from the intercode table
//only used for optimize
bool del_inter_code(ICN_Ptr *, InterCodePtr);

/* output the inter code table */
int output_inter_code_table(ICN_Ptr table);

void ir_optimize(ICN_Ptr *irhead);
/* get relop kind */
RelopKind get_relop_type(PtrTreeNode relop);
/* create new oprand */
OperandPtr new_function(char *func_name);
OperandPtr new_param(char *param_name);
OperandPtr new_address(int addr_no);
OperandPtr new_op(OpKind k, int id);
/* get the paramlist */
OperandPtr check_param(ICN_Ptr head, int id);
/* create new inter code*/
InterCodePtr new_ir_0(IrKind k, OperandPtr op);
InterCodePtr new_ir_1(IrKind k, OperandPtr l, OperandPtr r);
InterCodePtr new_ir_2(IrKind k, OperandPtr result, OperandPtr op1, OperandPtr op2);
InterCodePtr new_ir_cond(RelopKind relopKind, OperandPtr t1, OperandPtr t2, OperandPtr label);



/* translate functions */
ICN_Ptr translate_Exp(PtrTreeNode, OperandPtr);
ICN_Ptr translate_Stmt(PtrTreeNode);
ICN_Ptr translate_Cond(PtrTreeNode, OperandPtr, OperandPtr);
ICN_Ptr translate_Compst(PtrTreeNode);
ICN_Ptr translate_StmtList(PtrTreeNode);
ICN_Ptr translate_Args(PtrTreeNode, OperandPtr *, int *);
ICN_Ptr translate_ExtDef(PtrTreeNode);
ICN_Ptr translate_ExtDecList(PtrTreeNode);
ICN_Ptr translate_FunDec(PtrTreeNode);
ICN_Ptr translate_VarDec(PtrTreeNode, OperandPtr *);
ICN_Ptr translate_DefList(PtrTreeNode);
ICN_Ptr translate_Def(PtrTreeNode);
ICN_Ptr translate_DecList(PtrTreeNode);
ICN_Ptr translate_Dec(PtrTreeNode);

ICN_Ptr translate_Structure(PtrTreeNode, OperandPtr, FieldList *);
int get_type_size(Type);
#endif
