#include "./include/compiler.h"

//ExtDef --> Specifier ExtDecList SEMI | Specifier SEMI | Specifier FuncDec CompSt
//Def --> Specifier DecList SEMI
//tn is the child[0] of the left part of the above production, which is Specifier
void getSpecifier(Type type, PtrTreeNode tn){
	//Specifier --> TYPE
	//No name to judge
	if( strcmp(tn->child[0]->token, "TYPE")==0 ){
		//TYPE -> int
		PtrTreeNode TYPE = tn->child[0];
		if(strcmp(TYPE->ID, "int")==0 ){
			type->kind = basic;
			type->u.basic = 0;
		}
		//TYPE -> float
		else if(strcmp(TYPE->ID, "float")==0 ){
			type->kind = basic;
			type->u.basic = 1;
		}
	}
	//Specifier --> StructSpecifier
	//judge the structure name
	else{
		PtrTreeNode StructSpecifier = tn->child[0];
		type->kind = structure;
		//StructSpecifier --> STRUCT OptTag LC DefList RC  |  STRUCT Tag
		PtrTreeNode tag = StructSpecifier->child[1];
		//alloc space
		type->u.structure = (FieldList)malloc(sizeof(struct FieldList_));
		//StructSpecifier --> STRUCT Tag
		//Tag --> ID
		if( tag->token != NULL && strcmp( tag->token, "Tag") == 0 ){
			if(findstructType(tag->child[0]->ID) == 1){
				type->u.structure->name = malloc(32);
				memcpy(type->u.structure->name, tag->child[0]->ID, 32);
				type->u.structure->type = NULL;
				type->u.structure->tail = NULL;
			}
			//not in the Symbol table
			else
				printf("Error Type 17 at line %d : Undefined struct \"%s\"\n", tag->lineno, tag->child[0]->ID);
		}
		//StructSpecifier --> STRUCT OptTag LC DefList RC
		else{
			if(tag->token != NULL){
				type->u.structure->name = malloc(32);
				memcpy(type->u.structure->name, tag->child[0]->ID, 32);
			}

			else{
				type->u.structure->name = NULL;
			}
			type->u.structure->tail = NULL;

			PtrTreeNode DefList = StructSpecifier->child[3];
			//DefList --> Def DefList |
			while(DefList != NULL){
				//DefList --> Def DefList
				PtrTreeNode Def = DefList->child[0];

				//Def --> Specifier DecList SEMI
				PtrTreeNode Specifier = Def->child[0];
				Type type_recur = (Type)malloc(sizeof(struct Type_));
				getSpecifier(type_recur, Specifier);

				PtrTreeNode DecList = Def->child[1];
				//DecList --> Dec | Dec COMMA DecList
				while(DecList != NULL){
					//Dec --> VarDec | VarDec ASSIGNOP Exp
					PtrTreeNode VarDec = DecList->child[0]->child[0];
					FieldList tempfield = (FieldList)malloc(sizeof(struct FieldList_));
					tempfield->type = (Type)malloc(sizeof(struct Type_));                               
					//VarDec --> ID
					if(strcmp(VarDec->child[0]->token, "ID") == 0){
						tempfield->tail = NULL;
						tempfield->type->kind = type_recur->kind;
						//strcpy(tempfield->name,
						tempfield->name = (char*)malloc(32);
						strncpy(tempfield->name, VarDec->child[0]->ID,20);

						if(type_recur->kind == basic)
							tempfield->type->u.basic = type_recur->u.basic;
					}
					//VarDec --> VarDec LB INT RB
					//mul-array
					else{
						Type temp = (Type)malloc(sizeof( struct Type_ ));
						temp->kind = type_recur->kind;
						if(type_recur->kind == basic)
							temp->u.basic = type_recur->u.basic;

						PtrTreeNode node = VarDec->child[0];
						Type arraytype;
						//VarDec --> VarDec LB INT RB
						while(strcmp(node->token, "ID") != 0){
							arraytype = (Type)malloc(sizeof( struct Type_ ));
							arraytype->kind = array;
							arraytype->u.array.elem =temp;
							arraytype->u.array.size = VarDec->child[2]->int_value;
							temp = arraytype;
							node = node->child[0];
						}
						//VarDec --> ID
						//mul_array will be dealt as a linked list
						tempfield->type = arraytype;
						tempfield->name = (char*)malloc(32);
						strcpy(tempfield->name, node->ID);
						//printf("struct array member %s\n", tempfield->name);
						tempfield->tail = NULL;
					}
					//insert the tempfield
					FieldList iterator = type->u.structure->tail;
					FieldList pre = type->u.structure;
					/*
					 * 2013/05/25 by yyh
					 * fix a bug about the redefined var in the struct scope
					 */ 
					while(iterator != NULL){
						//printf("existed filed named .. %s\n", iterator->name);
						if(strcmp("ID", VarDec->child[0]->token) == 0){
							if(strcmp(iterator->name, VarDec->child[0]->ID) == 0)
								printf("Error type 15 at line %d: Redefined filed \'%s\'\n", VarDec->lineno, iterator->name);
						}
						iterator = iterator->tail;
						pre = pre->tail;
					}
					pre->tail = tempfield;
					if(DecList->childnum == 1){
						DecList = NULL;
					}
					//DecList --> Dec COMMA DecList
					else
						DecList = DecList->child[2];
				}
				//DefList --> Def DefList
				if(DefList->child[1] != NULL)
					DefList = DefList->child[1];
				else
					DefList = NULL;
			}

			if(structType_head == NULL ){
				
				structType_head = (structType)malloc(sizeof(struct structType_));
				structType_head->name = malloc(32);
				memcpy(structType_head->name, type->u.structure->name, 32);
				structType_head->tail = type->u.structure->tail;
				structType_head->next = NULL;
			}
			
			else{
				structType itr = structType_head;
				int test = 0;
				if(findstructType(type->u.structure->name) == 1 )
					test = 1;
				if(test == 0 ){
					itr->next = (structType)malloc(sizeof(struct structType_));
					itr->next->name = malloc(32);
					memcpy(itr->next->name, type->u.structure->name, 32);
					itr->next->tail = type->u.structure->tail;
					itr->next->next = NULL;
				}
				else{
					printf("Error Type 16 at line %d : Redefined Structure %s\n",tag->lineno,  type->u.structure->name);
				}
			}
		}
	}
}


void addVarDec(Type type, PtrTreeNode VarDec){
	//find the Variable
	SymbolNode newnode = (SymbolNode)malloc(sizeof(struct SymbolNode_));
	newnode->name = malloc(32);
	if(strcmp(VarDec->child[0]->token, "ID") != 0){
		Type elem = (Type)malloc(sizeof(struct Type_));
		elem->kind = type->kind;
		if(type->kind == basic)
			elem->u.basic = type->u.basic;

		Type arraytype;
		PtrTreeNode temp = VarDec->child[0];
		while(strcmp("ID", temp->token) != 0){
			arraytype = (Type)malloc(sizeof(struct Type_));
			arraytype->kind = array;
			arraytype->u.array.elem = elem;
			arraytype->u.array.size = VarDec->child[2]->int_value;
			//recursive deal with the array type
			elem = arraytype;
			temp = temp->child[0];
		}
		memcpy(newnode->name, temp->ID, 32);
		newnode->kind = var;
		newnode->right = NULL;
		newnode->down = NULL;
		newnode->u.type = arraytype;

	}
	/*
	 * VarDec --> ID
	 */
	else{
		int result = findName(VarDec->child[0]->ID);
		if(result==1 || result==2 || result ==3 ){
			printf("Error type 3 at line %d : Redefined variable \" %s \"\n", VarDec->lineno, VarDec->child[0]->ID);
			return;
		}
		//
		memcpy(newnode->name, VarDec->child[0]->ID, 32);
		newnode->kind = var;
		newnode->right = NULL;
		newnode->down = NULL;
		newnode->u.type = (Type)malloc(sizeof( struct Type_ ));
		newnode->u.type->kind = type->kind;
		if(type->kind == basic)
			newnode->u.type->u.basic = type->u.basic;
		else if(type->kind == array)
			newnode->u.type->u.array = type->u.array;
		else
			newnode->u.type->u.structure = type->u.structure;
	}
	/*
	 * insert the VarDec node
	 */
	symbol_insert(newnode);
}

/*
 * ExtDecList --> VarDec |  VarDec COMMA  ExtDecList
 */
void addExtDecList(Type type, PtrTreeNode ExtDecList){
	PtrTreeNode VarDec = ExtDecList->child[0];	
	if(ExtDecList->childnum > 1){
		Type typetemp = (Type)malloc(sizeof(struct Type_));
		memcpy(typetemp, type, sizeof(struct Type_));
		addExtDecList(typetemp, ExtDecList->child[2]);
	}
	addVarDec(type, VarDec);
}

//DecList --> Dec | Dec COMMA DecList
//Dec --> VarDec | VarDec ASSIGNOP Exp
void addDecList(Type type, PtrTreeNode DecList){
	PtrTreeNode Dec = DecList->child[0];
	if(DecList->childnum > 1){
		Type typetemp = (Type)malloc(sizeof(struct Type_));
		memcpy(typetemp, type, sizeof(struct Type_));
		addDecList(typetemp, DecList->child[2]);
	}
	PtrTreeNode VarDec = Dec->child[0];
	addVarDec(type, VarDec);
}

//VarList --> ParamDec COMMA VarList
//          | ParamDec
//ParamDec --> Specifier VarDec
/*
 * deal with the scope about the varlist
 */
void addVarList(Type type, PtrTreeNode VarList){
	PtrTreeNode ParamDec = VarList->child[0];
	PtrTreeNode VarDec = ParamDec->child[1];
	addVarDec(type, VarDec);
	if(VarList->childnum > 1){
		Type typetemp = (Type)malloc(sizeof(struct Type_));
		memcpy(typetemp, type, sizeof(struct Type_));
		addVarList(typetemp, VarList->child[2]);
	}
}

//ExtDef --> Specifier FuncDec CompSt
//FunDec --> ID LP VarList RP | ID LP RP
void addFunc(Type returnType, PtrTreeNode FunDec){
	if(findName(FunDec->child[0]->ID) == 4){
		printf("Error Type 4 at line %d: Redefined function \"%s\"\n", FunDec->lineno, FunDec->child[0]->ID);
		return;
	}
	PtrTreeNode ID = FunDec->child[0];
	SymbolNode newnode = (SymbolNode)malloc(sizeof(struct SymbolNode_));
	newnode->name = malloc(32);
	memcpy(newnode->name, ID->ID, 32);
	newnode->kind = func;
	newnode->right = NULL;
	newnode->down = NULL;
	newnode->u.func.returnType = (Type)malloc(sizeof( struct Type_ ));
	newnode->u.func.returnType->kind = returnType->kind;

	if(returnType->kind == basic)
		newnode->u.func.returnType->u.basic = returnType->u.basic;

	newnode->u.func.paramCount = 0;
	newnode->u.func.paramTypeList = NULL;
	//VarList -->ParamDec COMMA VarList  |  ParamDec
	if( FunDec->childnum > 3){
		PtrTreeNode VarList = FunDec->child[2];
		PtrTreeNode ParamDec = VarList->child[0];
		//ParamDec --> Specifier VarDec      
		while(ParamDec != NULL){
			PtrTreeNode Specifier = ParamDec->child[0];
			PtrTreeNode VarDec = ParamDec->child[1];
			Type tempList = (Type)malloc(sizeof(struct Type_));
			newnode->u.func.paramCount++;
			//call the getSpecifier() function
			getSpecifier(tempList, Specifier);
			/* 
			 * 2013/05/27
			 * fix the bug about the pro of func param scope 
			 */
			//VarDec -->ID | VarDec LB INT RB
			while(strcmp(VarDec->token, "ID") != 0)
				VarDec = VarDec->child[0];

			char *name = malloc(32);
			memcpy(name, VarDec->ID, 32);
			if(newnode->u.func.paramTypeList != NULL){
				FieldList field = newnode->u.func.paramTypeList;
				while(field->tail != NULL)
					field = field->tail;
				FieldList newfield = (FieldList) malloc( sizeof ( struct FieldList_ ));
				newfield->name = malloc(32);
				memcpy(newfield->name, name, 32);
				newfield->tail = NULL;
				newfield->type = (Type)malloc(sizeof( struct Type_ ));
				newfield->type->kind = tempList->kind;
				if(tempList->kind == basic)
					newfield->type->u.basic = tempList->u.basic;
				field->tail = newfield;
			}
			else{
				newnode->u.func.paramTypeList = (FieldList)malloc(sizeof( struct FieldList_));
				newnode->u.func.paramTypeList->name = malloc(32);
				memcpy(newnode->u.func.paramTypeList->name, name, 32);
				newnode->u.func.paramTypeList->type = (Type)malloc(sizeof( struct Type_ ));
				newnode->u.func.paramTypeList->tail = NULL;
				newnode->u.func.paramTypeList->type->kind = tempList->kind;
				if(tempList->kind == basic)
					newnode->u.func.paramTypeList->type->u.basic = tempList->u.basic;
				else if(tempList->kind == array ){
					newnode->u.func.paramTypeList->type->u.array.size = tempList->u.array.size;
					newnode->u.func.paramTypeList->type->u.array.elem = tempList->u.array.elem;
				}
				else if(tempList->kind == structure ){
					newnode->u.func.paramTypeList->type->u.structure = tempList->u.structure;
				}
			}

			if(VarList->child[2] != NULL){
				VarList = VarList->child[2];
				ParamDec = VarList->child[0];
			}
			else
				ParamDec = NULL;
		}
	}
	symbol_insert(newnode);
}
