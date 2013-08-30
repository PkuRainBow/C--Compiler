#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct SymbolNode_* SymbolNode;
typedef struct structType_* structType;

struct Type_{
	enum {basic, array, structure} kind;
	union{
		int basic;
		struct {Type elem; int size;} array;
		FieldList structure;
	}u;
};

struct FieldList_{
	char *name;
	Type type;
	FieldList tail;
};

struct SymbolNode_{
	char *name;
	union{
		struct{
			Type returnType;
			FieldList paramTypeList;
			int paramCount;
		}func;
		Type type;
	}u;
	enum {var, func} kind;
	//
	SymbolNode right;
	SymbolNode down;
};

struct Scope_node{
	//kind=1 func
        //kind=2 stmt
        int kind;
        char name[20];
	struct Scope_node *next;
	SymbolNode down;
};

struct Hash_head{
	int key;
	SymbolNode next;
};

struct structType_{
	 char* name;
	 FieldList tail;
	 structType next;
};

structType structType_head;
extern struct Scope_node* Scope_head;

extern void Scope_push(struct Scope_node* p);
extern void Scope_pop();
extern int symbol_insert(SymbolNode temp);
extern void init_symbol_table();
extern void delete_symbol_table();
extern unsigned int hash_pjw(char* name);
//kind==0 variable kind==1 func
extern int findName(char *name);

//
extern int findstructType(char *name);
extern structType getstructType(char *name);

extern SymbolNode getName(char *name);
extern void getSpecifier(Type type, PtrTreeNode tn);
extern void Exp_check(PtrTreeNode  Exp);
extern void addVarDec(Type type, PtrTreeNode VarDec);
extern void addExtDecList(Type type, PtrTreeNode ExtDecList);
extern void addDecList(Type type, PtrTreeNode DecList);
extern void addFunc(Type returnType, PtrTreeNode FunDec);

extern void Traverse(PtrTreeNode root);
//judge.c
extern int lValueJudge(struct TreeNode* Exp);
extern void getExpType(Type expType, struct TreeNode* currentNode);
extern int checkTypeEqual(Type firstType, Type thirdType);
extern int checkParamType(FieldList paramTypeList, struct TreeNode* Args);
extern void returnTypeJudge(struct TreeNode* Exp);

