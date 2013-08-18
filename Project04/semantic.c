#include "./include/compiler.h"

static struct Hash_head*  hash_head[65536];
struct Scope_node* Scope_head;
int var_index;
/*
 * insert a scope node
 */
void Scope_push(struct Scope_node* newnode){	
	if(Scope_head->next==NULL){
		Scope_head->next = newnode;
		newnode->next = NULL;
	}
	else{
		newnode->next = Scope_head->next;
		Scope_head->next = newnode;
	}
}

/*
 * delete a scope node
 */
void Scope_pop(){
	if(Scope_head->next==NULL) return;
	else{
		//debug
		//printf("scope_pop out node ...\n");
		struct Scope_node* newnode = Scope_head->next;
		Scope_head->next = newnode->next;
		//
		SymbolNode p = newnode->down;
		while(p != NULL){
			//debug
			//printf("scope pop free the %s ...\n", p->name);
			newnode->down = p->down;
			int key = hash_pjw(p->name);
			hash_head[key]->next = p->right;
			free(p);
			p = newnode->down;		
		}
		//printf("scope pop free the %s ...\n", newnode->name);
		free(newnode);
	}
}


int symbol_insert(SymbolNode temp){
	SymbolNode newnode = Scope_head->next->down;
	if(newnode == NULL){
		Scope_head->next->down = temp;
		temp->down = NULL;
	}
	else{
		while(newnode != NULL){    
			if(strcmp(newnode->name, temp->name)==0 ){
				return 0;
			}
			newnode = newnode->down;
		}
        newnode = temp;
        temp->down = NULL;
	}

	int key = hash_pjw(temp->name);
	if(hash_head[key]->next == NULL){
		hash_head[key]->next = temp;
		temp->right = NULL;
	}
	else{
		temp->right = hash_head[key]->next;
		hash_head[key]->next = temp;
	}
	
	if(temp->kind == var){
	        temp->var_no = var_index;
	        var_index++;
    }
	return 1;
}
/*
 * init the hash_table
 */
void init_symbol_table(){
	int i=0;
	var_index = 1;
	for(; i<65536; i++){
		hash_head[i] = malloc(sizeof(struct Hash_head));
		hash_head[i]->key = i;
		hash_head[i]->next = NULL;
	}
	Scope_head = (struct Scope_node*)malloc(sizeof(struct Scope_node));
        Scope_head->down = NULL;
        Scope_head->next = NULL;
	struct Scope_node* node = (struct Scope_node*)malloc(sizeof(struct Scope_node));
        node->down = NULL;
	    node->next = NULL;
        Scope_push(node);
        /*
         * insert function read and write node...
         */
    //define and insert the read()
    SymbolNode newnode1 = (SymbolNode)malloc(sizeof(struct SymbolNode_));
	newnode1->name = malloc(32);
	strncpy(newnode1->name, "read", 32);
	newnode1->kind = func;
	newnode1->right = NULL;
	newnode1->down = NULL;
	newnode1->u.func.returnType = (Type)malloc(sizeof( struct Type_ ));	
	newnode1->u.func.returnType->kind = basic;
	newnode1->u.func.returnType->u.basic = 0;
	newnode1->u.func.paramCount = 0;
	newnode1->u.func.paramTypeList = NULL;
    symbol_insert(newnode1);
    
    SymbolNode newnode2 = (SymbolNode)malloc(sizeof(struct SymbolNode_));
	newnode2->name = malloc(32);
	strncpy(newnode2->name, "write", 32);
	newnode2->kind = func;
	newnode2->right = NULL;
	newnode2->down = NULL;
	newnode2->u.func.returnType = (Type)malloc(sizeof( struct Type_ ));	
	newnode2->u.func.returnType->kind = basic;
	newnode2->u.func.returnType->u.basic = 0;
	newnode2->u.func.paramCount = 1;
	newnode2->u.func.paramTypeList = NULL;
	/* */
	newnode2->u.func.paramTypeList = (FieldList)malloc(sizeof( struct FieldList_));
	newnode2->u.func.paramTypeList->name = malloc(32);
	strncpy(newnode2->u.func.paramTypeList->name, "temp", 32);
	newnode2->u.func.paramTypeList->type = (Type)malloc(sizeof( struct Type_ ));
	newnode2->u.func.paramTypeList->tail = NULL;
	newnode2->u.func.paramTypeList->type->kind = basic;
    newnode2->u.func.paramTypeList->type->u.basic = 0;		
    symbol_insert(newnode2);   
    
}


void delete_symbol_table(){
	int i=0;
	for(; i<65536; i++){
		free(hash_head[i]);
	}
	Scope_pop();
	free(Scope_head);
}
//return 1 :find ok
//return 0 :find fail
/*
 * find the self_defined struct 
 */
int findstructType(char *name){
    structType itr = structType_head;
    while( itr != NULL ){ 
        if( strcmp(itr->name, name) == 0 ){
              return 1;
        }
        itr = itr->next;
    }
    return 0;
}

/*
 * get the self_defined struct 
 */
structType getstructType(char *name){
    structType itr = structType_head;
    while( itr != NULL ){ 
        if( strcmp(itr->name, name) == 0 ){
              return itr;
        }
        itr = itr->next;
    }
    return NULL;
}

int findName(char *name){
	int key = hash_pjw(name);
	SymbolNode newnode = hash_head[key]->next;
	if(newnode == NULL)
		return 0;
	else{
		char test=0;
		while(newnode !=NULL){
			if(strcmp(newnode->name, name)==0){
				test = 1;
				if(newnode->kind == 0){
					if(newnode->u.type->kind == 0)
						return 1;
					if(newnode->u.type->kind == 1)
						return 2;
					if(newnode->u.type->kind == 2)
						return 3;
                    //printf("find the var name\n");
				}
				else if(newnode->kind == 1){
					return 4;
                    //printf("find the func name\n");
                }
			}
			else
				newnode = newnode->right;
		}
	}
	return 0;
}

SymbolNode getName(char *name)
{
	int key = hash_pjw(name);
	SymbolNode newnode = hash_head[key]->next;
	if(newnode == NULL)
		return NULL;
	else{
		while(newnode !=NULL){
			if(strcmp(newnode->name, name)==0){
				return newnode;
			}
			else
				newnode = newnode->right;
		}
	}
	return NULL;
}
//hash function
//16 bits long INT value
unsigned int hash_pjw(char* name){
	unsigned long val = 0, i;
	for(; *name; ++name){
		val = (val<<2)+*name;
		if((i = val&(~0x3fff)))
			val = (val^(i>>12))&0x3fff;
	}
	return val;
}
