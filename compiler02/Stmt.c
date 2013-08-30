#include "Semantic.h"
#include<stdlib.h>
#include<string.h>

int lValueJudge(struct TreeNode* Exp)
{
    struct TreeNode* firstChild = Exp->child[0];
    if( strcmp( firstChild->token ,"ID" ) == 0 )
    {
        if( findName(firstChild->ID) != 4 )
            return 1;
        else
            return 0;
    }
    if( strcmp( firstChild->token, "INT" ) == 0 || strcmp( firstChild->token, "FLOAT" ) == 0 )
        return 0;
    if( strcmp( firstChild->token, "LP" ) == 0 || strcmp( firstChild->token, "MINUS" ) == 0 || strcmp( firstChild->token, "NOT") == 0)
        return lValueJudge(Exp->child[1]);
    if( Exp->child[1] != NULL && strcmp( Exp->child[1]->token, "ASSIGNOP") == 0) {
        if( lValueJudge(Exp->child[2]) == 0)
            return 0;
        if( lValueJudge(firstChild) == 0 )
            return 0;
        }
    return 1;
}


void getExpType(Type expType, struct TreeNode* currentNode)
{
   if( strcmp( currentNode->token, "INT" ) == 0 ){
       expType->kind = basic;
       expType->u.basic = 0;
       return ;
   }
   if( strcmp( currentNode->token, "FLOAT" ) == 0 ){
       expType->kind = basic;
       expType->u.basic = 1;
       return ;
   }
   if( strcmp( currentNode->token, "ID" ) == 0
		   && currentNode->parent->childnum == currentNode->index + 1){
	   SymbolNode ptr = getName(currentNode->ID);
        if( ptr == NULL ) {
            return ;
        }
        if( ptr->kind == var ){
            if( ptr->u.type->kind == basic ){
                expType->kind = basic;
                expType->u.basic = ptr->u.type->u.basic;
            }
            else if( ptr->u.type->kind == array ){
                expType->kind = array;
                expType->u.array = ptr->u.type->u.array;
            }
            else {
                expType->kind = structure;
                expType->u.structure = ptr->u.type->u.structure;
            }
        }
        else{
            expType->kind = ptr->u.func.returnType->kind;
            if( ptr->u.func.returnType->kind == basic ) {
                expType->kind = basic;
                expType->u.basic = ptr->u.func.returnType->u.basic;
            }
            else if ( ptr->u.func.returnType->kind == array ) {
                expType->kind = array;
                expType->u.array = ptr->u.func.returnType->u.array;
            }
            else{
                expType->kind = structure;
                expType->u.structure = ptr->u.func.returnType->u.structure;
            }
        }
        return ;
   }
   //
   if( strcmp( currentNode->token, "ID" ) == 0
		   && currentNode->parent->childnum != currentNode->index + 1){
	   SymbolNode ptr = getName(currentNode->ID);
        if( ptr == NULL || ptr->kind != func) {
            return ;
        }
		else{
			if( ptr->u.func.paramTypeList == NULL
					&& strcmp(currentNode->parent->child[currentNode->index+2]->token, "RP") == 0)
				return ;
            if( ptr->u.func.paramTypeList == NULL
					&& strcmp(currentNode->parent->child[currentNode->index+2]->token, "RP") != 0){
				printf("Error Type 9 at line %d: The funtion's paramlist is mismatched\n", currentNode->lineno);
                return;
			}
            if( ptr->u.func.paramTypeList != NULL
					&& strcmp(currentNode->parent->child[currentNode->index+2]->token, "RP") ==0){
				printf("Error Type 9 at line %d: The funtion's paramlist is mismatched\n", currentNode->lineno);
			return ;
			}
            if( checkParamType(ptr->u.func.paramTypeList, currentNode->parent->child[currentNode->index+2]) == 0 ){
                printf("Error Type 9 at line %d: The funtion's paramlist is mismatched\n", currentNode->lineno);
                return ;
            }
        }

   }
   if( strcmp( currentNode->token, "LP") == 0 || strcmp( currentNode->token, "MINUS") == 0 || strcmp( currentNode->token, "NOT") == 0 ){
	   getExpType(expType, currentNode->parent->child[currentNode->index+1]);
       return ;
   }
   if( strcmp( currentNode->token, "Exp" ) == 0 ){
       if( currentNode->parent->child[currentNode->index+1] != NULL
			   && strcmp(currentNode->parent->child[currentNode->index+1]->token, "ASSIGNOP") == 0 ){
		   if( lValueJudge(currentNode) == 0 ){
			   printf("Error Type 6 at line %d: The left-hand side of an assignment must be a variable\n", currentNode->lineno);
			   return ;
		   }
		   Type firstType = (Type)malloc(sizeof(struct Type_));
		   Type thirdType = (Type)malloc(sizeof(struct Type_));
		   getExpType(firstType, currentNode->child[0]);
		   getExpType(thirdType, currentNode->parent->child[currentNode->index+2]->child[0]);
           if( checkTypeEqual(firstType, thirdType) == 0 ){
               printf("Error Type 5 at line %d : Type mismatched\n", currentNode->lineno);
           }
		   return ;
	   }
	   if( currentNode->parent->child[currentNode->index + 1] != NULL
			   && strcmp(currentNode->parent->child[currentNode->index + 1]->token, "DOT") == 0 ){
           Type firstType = (Type)malloc(sizeof(struct Type_));
           getExpType(firstType, currentNode->child[0]);
           if( firstType->kind != structure )
               printf("Error Type 13 at line %d Illegal use of \".\"\n", currentNode->lineno);
       }
       if( currentNode->parent->child[currentNode->index + 1] != NULL
			   && (strcmp(currentNode->parent->child[currentNode->index + 1]->token, "AND") == 0 ||
				   strcmp(currentNode->parent->child[currentNode->index + 1]->token, "OR") == 0 ||
				   strcmp(currentNode->parent->child[currentNode->index + 1]->token, "RELOP") == 0 ||
                   strcmp(currentNode->parent->child[currentNode->index + 1]->token, "PLUS") == 0 ||
                   strcmp(currentNode->parent->child[currentNode->index + 1]->token, "MINUS") == 0 ||
				   strcmp(currentNode->parent->child[currentNode->index + 1]->token, "STAR") == 0 ||
                   strcmp(currentNode->parent->child[currentNode->index + 1]->token, "DIV") == 0 ))
	   {
           Type firstType = (Type)malloc(sizeof(struct Type_));
           Type thirdType = (Type)malloc(sizeof(struct Type_));
           getExpType(firstType, currentNode->child[0]);
           getExpType(thirdType, currentNode->parent->child[currentNode->index + 2]->child[0]);
           if( checkTypeEqual(firstType, thirdType) == 0 ){
               printf("Error Type 7 at line %d:Operands type mismatched\n", currentNode->lineno);
           }
           return ;
       }
       if( currentNode->parent->child[currentNode->index + 1] != NULL
			   && (strcmp(currentNode->parent->child[currentNode->index + 1]->token, "LB") == 0)){
           Type firstType = (Type)malloc(sizeof(struct Type_));
           Type thirdType = (Type)malloc(sizeof(struct Type_));
           getExpType(firstType, currentNode->child[0]);
           getExpType(thirdType, currentNode->parent->child[currentNode->index + 2]->child[0]);
           if( firstType->kind != array ){
               printf("Error Type 10 at line %d:\"%s\" must be an array\n", currentNode->lineno, currentNode->child[0]->ID);
               expType->kind = firstType->kind;
               if(expType->kind == basic )
                   expType->u.basic = firstType->u.basic;
               else
                   expType->u.structure = firstType->u.structure;
               return ;
           }
           else{
              if(thirdType->kind != basic || thirdType->u.basic != 0 ){
                  printf("Error Type 12 at line %d: Operands type mistaken\n", currentNode->lineno);
                  expType->kind  = firstType->u.array.elem->kind;
                  if( expType->kind == basic )
                      expType->u.basic = firstType->u.array.elem->u.basic;
                  else if ( expType->kind == array )
                      expType->u.array = firstType->u.array.elem->u.array;
                  else
                      expType->u.structure = firstType->u.array.elem->u.structure;
              }
              else {
                  expType->kind  = firstType->u.array.elem->kind;
                  if( expType->kind == basic )
                      expType->u.basic = firstType->u.array.elem->u.basic;
                  else if ( expType->kind == array )
                      expType->u.array = firstType->u.array.elem->u.array;
                  else
                      expType->u.structure = firstType->u.array.elem->u.structure;
              }
           }
           return ;
       }
       getExpType(expType, currentNode->child[0]);
   }
   return ;
}

int checkTypeEqual(Type firstType, Type thirdType)
{
    if( firstType->kind != thirdType->kind )
        return 0;
    if( firstType->kind == basic ){
        if( firstType->u.basic == thirdType->u.basic )
            return 1;
        else
            return 0;
    }
    if( firstType->kind == array ){
            return checkTypeEqual(firstType->u.array.elem, thirdType->u.array.elem);
    }
    if( firstType->kind == structure ){
        if( strcmp(firstType->u.structure->name, thirdType->u.structure->name) == 0 )
            return 1;
        else
            return 0;
    }
    return 1;
}

int checkParamType(FieldList paramTypeList, struct TreeNode* Args)
{
    FieldList pitr = paramTypeList;
    struct TreeNode* titr = Args;
    if( pitr == NULL && titr == NULL )
        return 1;
    if( pitr != NULL && titr != NULL ){
        Type param = pitr->type;

        Type expType = (Type)malloc(sizeof(struct Type_));
        getExpType(expType, titr->child[0]);
        if( checkTypeEqual(param, expType) == 1 ){
            pitr = pitr->tail;
            if( titr->child[0]->parent->child[titr->child[0]->index + 1] == NULL )
                titr = NULL;
            else
                titr = titr->child[0]->parent->child[titr->child[0]->index + 2];
            return checkParamType(pitr, titr);
        }
        else {
            return 0;
        }
    }
    return 0;
}
void returnTypeJudge(struct TreeNode* Exp)
{
	struct Scope_node * tempnode = Scope_head->next;
	while(tempnode != NULL && tempnode->kind != 1)
		tempnode = tempnode->next;
	SymbolNode funNode = getName(tempnode->name);
	Type type = (Type)malloc(sizeof(struct Type_));
	getExpType(type, Exp);
        Type returnType = funNode->u.func.returnType;
        if( checkTypeEqual(type, returnType) == 0 )
             printf("Error Type 8 at line %d: return type mismatched\n", Exp->lineno);
}

void Exp_check(PtrTreeNode  Exp){
	PtrTreeNode temp = Exp->child[0];
        if(strcmp(temp->token, "Exp") == 0){
                Exp_check(temp);
                if(strcmp(Exp->child[1]->token, "DOT")==0){
                    Type type = (Type)malloc(sizeof(struct Type_));
                    getExpType(type, Exp->child[0]);
                    structType temp_struct = getstructType(type->u.structure->name);
                    FieldList head = temp_struct->tail;
                    int test = 0;
                    while(head!=NULL){
                        if(strncmp(head->name, Exp->child[2]->ID, 32)==0)   test = 1;
                         head = head->tail;
                    }
                    if(test == 0){
	                 printf("Error type 14 : Unexisted field \"%s\"\n", Exp->child[2]->ID);
		    }
                          
                }
                else{
                    Exp_check(Exp->child[2]);
                }
        }
	if(strcmp(temp->token, "ID") == 0){
                int result = findName(temp->ID);
		if(result == 0){
			if(Exp->childnum > 1)
				printf("Error Type 2 at line %d: Undefined function \"%s\"\n",temp->lineno, temp->ID);
			else
				printf("Error Type 1 at line %d: Undefined variable \"%s\"\n",temp->lineno, temp->ID);
		}
		else if(Exp->childnum > 1){
                     if(result != 4)
			printf("Error Type 11 at line %d : \"%s\" must be a funtion\n", temp->lineno, temp->ID);
}
	}
	else if(strcmp(temp->token, "LP") == 0 || strcmp(temp->token, "MINUS") == 0
			|| strcmp(temp->token, "NOT") == 0)
		Exp_check(Exp->child[1]);
}


