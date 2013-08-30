#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#define MAXCHILD 7

struct TreeNode{
      int isToken;
	int lineno;
	int childnum;
	struct TreeNode* parent;
	struct TreeNode* child[MAXCHILD];
	char token[20];
      int int_value;
      float  float_value;
      char ID[32];//ID TYPE
};

typedef struct TreeNode TreeNode, *PtrTreeNode;

extern void Insert(struct TreeNode* root, struct TreeNode* tn);
extern void FreeTree(struct TreeNode* root);
extern void PrintTree(struct TreeNode* root);

