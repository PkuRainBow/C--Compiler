#include "tree.h"

void Insert(PtrTreeNode root, PtrTreeNode tn){       
        if(tn==NULL) {          
             return;
        }
        int childnum = root->childnum;	
	tn->index = childnum;
        tn->parent = root;
        root->child[childnum] = tn;
	root->child[childnum+1] = NULL;
        root->childnum++;
        root->lineno = root->child[0]->lineno;
        return;
}

void FreeTree(PtrTreeNode root){
	int i;
	if(root==NULL) return;
	for(i=0; i<root->childnum; i++){
		FreeTree(root->child[i]);
	}
	free(root);
}

void PrintTree(PtrTreeNode root){
   static int depth = 0;
   int i;
   if(root == NULL) return;
   for(i=0; i<depth; i++) printf(" ");
   
   if(!root->isToken){
       printf("%s (%d)\n", root->token, root->lineno);
   }
   else{
         if(strncmp(root->token, "INT", 20) == 0)
             printf("%s: %d\n", root->token, root->int_value);
         else if(strncmp(root->token, "FLOAT", 20) == 0)
             printf("%s: %f\n", root->token, root->float_value);
         else if(strncmp(root->token,"TYPE", 32) == 0 || strncmp(root->token,"ID", 32) == 0)
	         printf("%s: %s\n", root->token, root->ID);
         else
             printf("%s\n", root->token);
   }
   for(i=0; i<root->childnum; i++){
	   depth++;
	   PrintTree(root->child[i]);
	   depth--;
   }
}
