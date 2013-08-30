#ifndef __TREE_H__
#define __TREE_H__

#define MAXCHILD 9

struct TreeNode{
    int isToken;
	int lineno;
	int childnum;
	int index;//
	struct TreeNode* parent;
	struct TreeNode* child[MAXCHILD];
	char token[20];
    int int_value;
    float  float_value;
    char ID[32];//ID TYPE
	unsigned int rule;
};

typedef struct TreeNode TreeNode, *PtrTreeNode;

extern void Insert(struct TreeNode* root, struct TreeNode* tn);
extern void FreeTree(struct TreeNode* root);
extern void PrintTree(struct TreeNode* root);
#endif
