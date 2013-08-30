#include "./include/compiler.h"


ICN_Ptr ir_table;
extern FILE *fout;

//bind 2 codelist 
ICN_Ptr bind_inter_code(ICN_Ptr code1, ICN_Ptr code2)
{
	ICN_Ptr temp = code1;
	if(code1 == NULL)
		return code2;
	if(code2 == NULL)
		return code1;

	while(temp->next != NULL)
		temp = temp->next;
	temp->next = code2;
	code2->prev = temp;
	return code1;
}


//add the inter_code into the code table tail
void add_inter_code(ICN_Ptr *table_head, InterCodePtr inter_code)
{
	ICN_Ptr codelist = (ICN_Ptr) malloc(sizeof (InterCodeNode));
	ICN_Ptr temp;
	codelist->code = inter_code;

	assert( table_head != NULL);
	if( *table_head == NULL){
		*table_head = codelist;
		codelist->prev = NULL;
		codelist->next = NULL;
	}else{
		temp = *table_head;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = codelist;
		codelist->prev = temp;
		codelist->next = NULL;
	}
}

//delete the inter_code from the table, noting that the table may be modified.
bool del_inter_code(ICN_Ptr *table_head, InterCodePtr inter_code)
{
	ICN_Ptr temp;
	ICN_Ptr head = *table_head;
	assert(table_head != NULL);
	if(head == NULL)
		return false;
	else{
		while(head != NULL){
			if(head->code == inter_code){
				if(head->prev == NULL){
					*table_head = head->next;
					(*table_head)->prev = NULL;
					free(head);
				}else{
					temp = head;
					temp->next->prev = head->prev;
					head->prev->next = temp->next;
					free(temp);
				}
				return true;
			}
			head = head->next;
		}
		return false;
	}
}

// output the operand
int output_operand(OperandPtr op, int star) 
{
	//int id; don't use

	if (op == NULL) {
		return 0;
	}

	if (op->kind == VARIABLE ) {
			fprintf(fout,"v%d", op->u.var_no);
	}
	
	else if (op->kind == CONSTANT) {
		fprintf(fout, "#%d", op->u.value);
	}
	else if (op->kind == FUNCTION) {
		fprintf(fout, "%s", op->u.func_name);
	}
	else if (op->kind == OP_LABEL) {
		fprintf(fout, "%s", op->u.label_name);
	}
	else if (op->kind == ADDRESS) {
		if (star == 1) 
			fprintf(fout,"*");
			fprintf(fout,"t%d", op->u.var_no);
	}
	else if (op->kind == TEMP) {
		fprintf(fout,"t%d", op->u.var_no);
	}
	else if (op->kind == ADDRESS_GET) {
		//find if the op is parameter, needn't &
		if (check_param(ir_table, op->u.addr_no) == NULL) {
			fprintf(fout,"&v%d", op->u.addr_no);
		}
		else {
			fprintf(fout,"v%d", op->u.addr_no);
			op->kind = TEMP;
			op->u.var_no = op->u.addr_no;
		}
	}
	return 1;
}

// 1 Operand
int output_operand_0(char *c, OperandPtr op) 
{
    fprintf(fout,"%s", c);
    output_operand(op, 1);
    fprintf(fout,"\n");
    return 1;
}

// 2 Operand
int output_operand_1(char *c, OperandPtr op1, OperandPtr op2, int newline) 
{
    if (op1 == NULL && strcmp(c, " := ") == 0) 
        return 0;
	if( op1 != NULL)
		output_operand(op1, op1->kind == ADDRESS);
	fprintf(fout,"%s", c);
	output_operand(op2, op2->kind == ADDRESS);
	if (newline == 1) fprintf(fout,"\n");
    return 1;
}
// 2 Operand + result
int output_operand_2(char *c, OperandPtr result, OperandPtr op1, OperandPtr op2) 
{
	if(result != NULL){
        output_operand(result, result->kind == ADDRESS);
        
        fprintf(fout," := ");
    }

    output_operand(op1, op1->kind == ADDRESS);
    fprintf(fout,"%s", c);
    
    output_operand(op2, op2->kind == ADDRESS);
    fprintf(fout,"\n");
    return 1;
}
/*
 *output the inter code  
 */
int output_inter_code_table(ICN_Ptr table) 
{
	ICN_Ptr icn = table;
	InterCodePtr icp;
	int tmp;
	char relop_type[10];
	int i = 1;
	while (NULL != icn) {
		icp = icn->code;
		assert(icp != NULL); 
		switch (icp->kind) {
			case _ASSIGN:
				output_operand_1(" := ", icp->u.assign.left,
						icp->u.assign.right, 1);
				break;
			case _ADD:
				output_operand_2(" + ", icp->u.binop.result,
						icp->u.binop.op1, icp->u.binop.op2);
				break;
			case _SUB:
				output_operand_2(" - ", icp->u.binop.result,
						icp->u.binop.op1, icp->u.binop.op2);
				break;
			case _DIV:
				output_operand_2(" / ", icp->u.binop.result,
						icp->u.binop.op1, icp->u.binop.op2);
				break;
			case _MUL:
				output_operand_2(" * ", icp->u.binop.result,
						icp->u.binop.op1, icp->u.binop.op2); 
				break;
			case _CALL:
				if (NULL != icp->u.assign.left)
					output_operand_1(" := CALL ", icp->u.assign.left, icp->u.assign.right, 1);
				else 
					output_operand_1("CALL ", NULL, icp->u.assign.right, 1);
				break;
			case _RELOP:
				fprintf(fout,"IF ");
				switch(icp->u.cond.reltype) {
					case RELOP_EQ: strcpy(relop_type, " == "); break;
					case RELOP_NE: strcpy(relop_type, " != "); break;
					case RELOP_GT: strcpy(relop_type, " > ");  break;
					case RELOP_LT: strcpy(relop_type, " < ");  break;
					case RELOP_GE: strcpy(relop_type, " >= "); break;
					case RELOP_LE: strcpy(relop_type, " <= "); break;
					default: break;
				}
				output_operand_1(relop_type, icp->u.cond.relop1, icp->u.cond.relop2, 0);
				fprintf(fout," GOTO ");
				fprintf(fout,"%s\n", (icp->u.cond.label)->u.label_name);
				break;
			case IR_LABEL:
				fprintf(fout,"LABEL %s :\n", icp->u.uniop.op->u.label_name);
				break;
			case _RETURN:
				output_operand_0("RETURN ", icp->u.uniop.op);
				break;
			case _GOTO:
				output_operand_0("GOTO ", icp->u.uniop.op);
				break;
			case VAR_DEC:
				fprintf(fout,"DEC  ");
				output_operand(icp->u.assign.left, 0);
				fprintf(fout," %d\n", icp->u.assign.right->u.value);
				break;
			case _READ:
				output_operand_0("READ ", icp->u.uniop.op);
				//fprintf(fout,"%d", icp->u.uniop.op->u.var_no);
				break;

			case _WRITE:
				output_operand_0("WRITE ", icp->u.uniop.op);
				break;
			case DEF_FUNCTION:
				fprintf(fout,"FUNCTION %s :\n", icp->u.uniop.op->u.func_name);
				break;
			case IR_PARAM:
				tmp = getName((icp->u.uniop.op)->u.param_name)->var_no;
				fprintf(fout,"PARAM v%d\n", tmp);
				break;
			case _ARG:
				output_operand_0("ARG ", icp->u.uniop.op);
				break;
			default:
				fprintf(fout,"Unknown IR\n");
				exit(-1);
		}
		icn = icn->next;
	}
	return 1;
}

//generate the irtable ^_^
int generate_ir(PtrTreeNode node)
{
	ICN_Ptr current_ic = NULL;
	if(node == NULL)
		return 0;

	if(strncmp(node->token , "ExtDef", 20) == 0){
		current_ic = translate_ExtDef(node);
		ir_optimize(&current_ic);
		ir_table = bind_inter_code(ir_table, current_ic);
		output_inter_code_table(current_ic);
		return 0;
	}

}


/* get relop type*/
RelopKind get_relop_type(PtrTreeNode relop) {
    char *value = relop->ID;
    if (0 == strcmp(value, "==")) {
        return RELOP_EQ;
    }
    else if (0 == strcmp(value, "!=")) {
        return RELOP_NE; 
    }
    else if (0 == strcmp(value, "<")) {
        return RELOP_LT;
    }
    else if (0 == strcmp(value, ">")) {
        return RELOP_GT;
    }
    else if (0 == strcmp(value, "<=")) {
        return RELOP_LE;
    }
    else if (0 == strcmp(value, ">=")) {
        return RELOP_GE;
    }
    else {
        fprintf(fout,"Unknown relop type!\n");
        exit(-1);
    }
}

/*
 * creat OperandPtr 
 */
// create a new function Operand 
OperandPtr new_function(char *func_name) 
{
    OperandPtr func = (OperandPtr)malloc(sizeof(Operand));
    func->kind = FUNCTION;
    func->u.func_name = func_name;
    return func;
}
// create a param of function
OperandPtr new_param(char *param_name) 
{
    OperandPtr param = (OperandPtr)malloc(sizeof(Operand));
    param->kind = OP_PARAM;
    param->u.param_name = param_name;
    return param;
}

OperandPtr new_address(int addr_no) 
{
    OperandPtr addr = (OperandPtr)malloc(sizeof(Operand));
    addr->kind = ADDRESS_GET; 
    addr->u.addr_no = addr_no;
    return addr;
}

OperandPtr new_op(OpKind k, int id) 
{
    OperandPtr op = (OperandPtr)malloc(sizeof(Operand));
    op->kind = k;
    if (k == TEMP || k == VARIABLE || k == ADDRESS) {
        op->u.var_no = id;
    }
	else if( k == CONSTANT){
        op->u.value = id;
    }
    return op;
}

/* check if it's a param */
OperandPtr check_param(ICN_Ptr head, int id) {
    ICN_Ptr tmp = head;
    InterCodePtr ic;

    if (head == NULL) return NULL;
    while (NULL != tmp) {
        ic = tmp->code;
        if (ic->kind == IR_PARAM) {
            if (getName((ic->u.uniop.op)->u.param_name)->var_no == id) {
                return ic->u.uniop.op;
            }
        }
        tmp = tmp->next;
    }
    return NULL;
}

/*
 * creat inter code
 */
InterCodePtr new_ir_0(IrKind k, OperandPtr op) 
{
    InterCodePtr code2 = (InterCodePtr)malloc(sizeof(InterCode));
    code2->kind = k;
    code2->u.uniop.op = op;
    return code2;
}

InterCodePtr new_ir_1(IrKind k, OperandPtr l, OperandPtr r) 
{
    InterCodePtr code2 = (InterCodePtr)malloc(sizeof(InterCode));
    code2->kind = k;
    code2->u.assign.left = l;
    code2->u.assign.right = r;
    return code2;
}

InterCodePtr new_ir_2(IrKind k, OperandPtr result, OperandPtr op1, OperandPtr op2) 
{
    InterCodePtr code2 = (InterCodePtr)malloc(sizeof(InterCode));
    code2->kind = k;
    code2->u.binop.result = result;
    code2->u.binop.op1 = op1;
    code2->u.binop.op2 = op2;
    return code2;
}

InterCodePtr new_ir_cond(RelopKind relopKind, OperandPtr t1, OperandPtr t2, OperandPtr label) 
{
    InterCodePtr code2 = (InterCodePtr)malloc(sizeof(InterCode));
    code2->kind = _RELOP;
    code2->u.cond.reltype = relopKind;
    code2->u.cond.relop1 = t1;
    code2->u.cond.relop2 = t2;
    code2->u.cond.label = label;
    return code2;
}

/* get the type size to declare the space the structure or the array need*/
int get_type_size(Type type) {
	assert(type != NULL);
    Type array_type;
    FieldList member;
    int size;

    switch(type->kind) {
    case basic:
        return 4;
    case array:
        array_type = type;
        size = 1;
        while (basic != array_type->kind) {
            size *= array_type->u.array.size;
            array_type = array_type->u.array.elem;
        }
        return size * get_type_size(array_type); 
    case structure:
        member = type->u.structure;
        size = 0;
        while (NULL != member) {
            size += get_type_size(member->type);
            member = member->tail;
        }
        return size;
    default:
        return 0;
    }
}

/* traverse the tree and translate the tree */
void IRTraverse(struct TreeNode* head)
{
    struct TreeNode *child;
    int i;

	if(strcmp(head->token, "Program") == 0){
		IRTraverse(head->child[0]);
	}
	else if(head->token != NULL){
		if( strcmp(head->token, "ExtDef") == 0 ){
			generate_ir(head);
        }

        if(head->child[0] != NULL) {
            IRTraverse(head->child[0]);
        } 
        if(head->parent->child[head->index+1] != NULL){
            IRTraverse(head->parent->child[head->index+1]);
        }
    }
    else{
        if(head->parent->child[head->index+1] != NULL)
            IRTraverse(head->parent->child[head->index+1]);
    }
}

