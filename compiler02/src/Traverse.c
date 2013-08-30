#include "Semantic.h"

/******************   DFS    ****************/
void Traverse(PtrTreeNode root){
	PtrTreeNode temp;
	//printf("start\n");
	if(strcmp(root->token, "Program") == 0){
		Traverse(root->child[0]);
	}
	else if(root->token != NULL){
		if((strcmp(root->token, "FunDec") == 0) && (strcmp(root->parent->token, "ExtDef") == 0)  ){
			//debug
			/*
			 * 
			 */
			//add Func
			PtrTreeNode specifier = root->parent->child[0];
			Type type = (Type)malloc(sizeof(struct Type_));
			getSpecifier(type, specifier);
			addFunc(type, root);
			//add Scope node
			struct Scope_node* newnode = (struct Scope_node*)malloc(sizeof(struct Scope_node));
			newnode->kind = 1;
			strncpy(newnode->name, root->child[0]->ID, 20);
			Scope_push(newnode);                        
			//add VarList
			if(root->childnum > 3){
				PtrTreeNode specifier = root->child[2]->child[0]->child[0];
				Type type = (Type)malloc(sizeof(struct Type_));
				getSpecifier(type, specifier);
				addVarList(type, root->child[2]);
			}                
		}

		if((strcmp(root->token, "CompSt") == 0) &&(strcmp(root->parent->token, "Stmt") == 0)  ){
			struct Scope_node* newnode = (struct Scope_node*)malloc(sizeof(struct Scope_node));
			newnode->kind = 2;
			memset(newnode->name,0,20);
			Scope_push(newnode);
		}

		if(strcmp(root->token, "ExtDef") == 0){
			PtrTreeNode specifier = root->child[0];
			Type type = (Type)malloc(sizeof(struct Type_));
			getSpecifier(type, specifier);
			if( strcmp(root->child[1]->token, "ExtDecList") == 0){
				addExtDecList(type, root->child[1]);
			}
		}		


		if( (strcmp(root->token, "CompSt") == 0) && (strcmp(root->child[1]->token,"DefList")==0)){
			PtrTreeNode DefList = root->child[1];
			while(DefList != NULL){
				if(DefList->child[0]==NULL){
					break;
				}
				PtrTreeNode Def = DefList->child[0];
				PtrTreeNode specifier = Def->child[0];
				Type type = (Type)malloc(sizeof(struct Type_));
				getSpecifier(type, specifier);
				PtrTreeNode DecList = Def->child[1];
				addDecList(type, DecList);
				if(DefList->childnum < 2) break;
				DefList = DefList->child[1]; 
			}
		} 

		if((strcmp(root->token, "RC") == 0) &&(strcmp(root->parent->token, "CompSt") == 0)){
			Scope_pop();
		}

		if(strcmp(root->token, "RETURN") == 0){
			returnTypeJudge(root->parent->child[root->index+1]);
		}

		if(strcmp(root->token, "Exp") == 0){
			Exp_check(root);
			Type type = (Type)malloc(sizeof(struct Type_));
			getExpType(type, root);
			if(root->parent->child[root->index + 1] != NULL
					&& strcmp(root->parent->child[root->index + 1]->token, "RP") == 0){
				if(root->parent->child[root->index + 2] != NULL
						&& strcmp(root->parent->child[root->index + 2]->token, "Stmt") == 0)
					Traverse(root->parent->child[root->index+2]);				
			}
			return;
		}

		if(root->child[0] != NULL){
			Traverse(root->child[0]);
		}

		if(root->parent->childnum-1 > root->index){
			Traverse(root->parent->child[root->index+1]);
		}    
	}

	else{
		if(root->parent->childnum-1 > root->index){
			Traverse(root->parent->child[root->index+1]);
		}
	}
}

