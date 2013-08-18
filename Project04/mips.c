#include "./include/mips.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern ICN_Ptr ir_table;
extern FILE *fout;
MIPSnode* mips_addr_list = NULL;

int arg_count = 0, param_count = 0, define_count = 0;

void output_mips()
{
	read_write_code();
	ICN_Ptr temp = ir_table;
	while(temp != NULL){
		if (temp->code->kind==DEF_FUNCTION)
			record_alloc(temp->next, ir_table);
		print_mips_code(temp->code);
		temp = temp->next;
	}
}

void record_alloc(ICN_Ptr icn, ICN_Ptr codes)
{
	define_count = 0;
	param_count = 0;
	mips_addr_list = NULL;
	while (icn != NULL && icn->code->kind != DEF_FUNCTION) {
		switch (icn->code->kind) {
			case _ASSIGN:
				if(icn->code->u.assign.left == NULL) break;
				insert_op(icn->code->u.assign.right);
				insert_op(icn->code->u.assign.left);
				break;
			case _ADD:
			case _SUB:
			case _MUL:
			case _DIV:
				insert_op(icn->code->u.binop.op1);
				insert_op(icn->code->u.binop.op2);
				insert_op(icn->code->u.binop.result);
				break;
			case _RELOP:
                insert_op(icn->code->u.cond.relop1);
				insert_op(icn->code->u.cond.relop2);
				break;

 
            //**********************************************

            //LOAD WRITE

            //**********************************************

			case IR_PARAM:
				param_count ++;
				insert_mips_node(icn->code->u.uniop.op->u.param_name, param_count * 4 + 4);
				break;

            //********************************
           //*********************************

			case VAR_DEC:
				define_count += icn->code->u.assign.right->u.value / 4;
				char name[20];
				sprintf(name, "v%d", icn->code->u.assign.left->u.var_no);
				insert_mips_node(name, define_count * (-4));
				break;
			case _CALL:
				insert_op(icn->code->u.assign.left);
			case _READ:
			case _WRITE:
				insert_op(icn->code->u.uniop.op);
				break;
			default:break;
		}
		icn = icn->next;
	}
}

void print_mips_code(InterCodePtr code) 
{
	char opt[20];
	switch (code->kind) {
		//struct { Operand *func_name; } func;
		case DEF_FUNCTION:
			fprintf(fout, "%s:\n", code->u.uniop.op->u.func_name);
			fprintf(fout, "  move $fp, $sp\n");
			arg_count = 0;
			fprintf(fout, "  addi $sp, $sp, %d\n", define_count * (-4));
			break;
		case IR_LABEL:
			fprintf(fout, "%s:\n", code->u.uniop.op->u.label_name);
			break;
		case _ASSIGN:
			//LOAD
			if(code->u.assign.left == NULL)break;
			if(code->u.assign.right->kind == ADDRESS){
				load("$t0", code->u.assign.right);
				fprintf(fout, "  lw $t2, 0($t0)\n");
				store("$t2", code->u.assign.left);
			}
			//STORE
			else if(code->u.assign.left->kind == ADDRESS){
				load("$t2", code->u.assign.right);
				load("$t0", code->u.assign.left);
				fprintf(fout, "  sw $t2, 0($t0)\n");
			}
			else{
				load("$t0", code->u.assign.right);
				fprintf(fout, "  move $t2, $t0\n");
				store("$t2", code->u.assign.left);
			}
			break;
		case _ADD:
		case _SUB:
		case _MUL:
		case _DIV:
			if (code->kind == _ADD) strcpy(opt, "add");
			else if (code->kind == _SUB) strcpy(opt, "sub");
			else if (code->kind == _MUL) strcpy(opt, "mul");
			else if (code->kind == _DIV) strcpy(opt, "div");

			load("$t0", code->u.binop.op1);
			load("$t2", code->u.binop.op2);
			fprintf(fout, "  %s $t2, $t0, $t2\n", opt);
			store("$t2", code->u.binop.result);
			break;
		case _RELOP:
			switch(code->u.cond.reltype) {
                case RELOP_EQ: strcpy(opt, "beq"); break;
				case RELOP_NE: strcpy(opt, "bne"); break;
				case RELOP_GT: strcpy(opt, "bgt");  break;
				case RELOP_LT: strcpy(opt, "blt");  break;
				case RELOP_GE: strcpy(opt, "bge"); break;
				case RELOP_LE: strcpy(opt, "ble"); break;
				default: break;
            }
			load("$t2", code->u.cond.relop1);
			load("$t0", code->u.cond.relop2);
			fprintf(fout, "  %s $t2, $t0, %s\n", opt, (code->u.cond.label)->u.label_name);
			break;


        //branch .----->  goto
		case _GOTO:
			fprintf(fout, "  j %s\n", code->u.uniop.op->u.label_name);
			break;

	
		case _RETURN:
			load("$t2", code->u.uniop.op);
			fprintf(fout, "  move $v0, $t2\n");
			fprintf(fout, "  move $sp, $fp\n");
			fprintf(fout, "  jr $ra\n");
			break;
		case VAR_DEC:
			break;
		case _ARG:
			arg_count ++;
			load("$t2", code->u.uniop.op);
			stack_push("$t2");
			break;
		case _CALL:
			stack_push("$fp");
			stack_push("$ra");
			fprintf(fout, "  jal %s\n", code->u.assign.right->u.func_name);//**************
			stack_pop("$ra");
			stack_pop("$fp");
			fprintf(fout, "  addi $sp, $sp, %d\n", arg_count * 4);
			store("$v0", code->u.assign.left);//*****************
			arg_count = 0;
			break;
			//*************
		case _READ:
			stack_push("$ra");
			fprintf(fout, "  jal read\n");
			stack_pop("$ra");
			store("$v0", code->u.uniop.op);
			break;
		case _WRITE:
			load("$a0", code->u.uniop.op);
			stack_push("$ra");
			fprintf(fout, "  jal write\n");
			stack_pop("$ra");
			break;
		default:break;
	}
}

void stack_push(char* reg) {
	fprintf(fout, "  addi $sp, $sp, -4 \n");
	fprintf(fout, "  sw %s 0($sp) \n", reg);
}

void stack_pop(char* reg) {
	fprintf(fout, "  lw %s 0($sp) \n", reg);
	fprintf(fout, "  addi $sp, $sp, 4 \n");
}

MIPSnode *new_MIPSnode(char *var, int addr) {
	MIPSnode *van = (MIPSnode*)malloc(sizeof(MIPSnode));
	van->next = NULL;
	strcpy(van->var, var);
	van->addr = addr;
	return van;
}

MIPSnode *get_mips_node(char* var) {
	MIPSnode *tvan = mips_addr_list;
	while (tvan != NULL) {
		assert(tvan->var != NULL);
		if (strcmp(var, tvan->var) == 0)
			return tvan;
		tvan = tvan->next;
	}
	return NULL;
}

void insert_mips_node(char *var, int addr) {
	MIPSnode *van = new_MIPSnode(var, addr);
	van->next = mips_addr_list;
	mips_addr_list = van;
}

void insert_op(OperandPtr op)
{
	char name[20];
	if (op->kind == CONSTANT) return;
	if(op->kind == VARIABLE){
		sprintf(name, "v%d", op->u.var_no);
	}
	else if(op->kind == TEMP){
		sprintf(name, "t%d", op->u.var_no);
	}
	else if(op->kind == ADDRESS){
		sprintf(name, "t%d", op->u.var_no);
	}
	else if(op->kind == ADDRESS_GET){
		sprintf(name, "t%d", op->u.var_no);
	}
	//ADDRESS
	MIPSnode *van = get_mips_node(name);
	if (van == NULL) {
		define_count ++;
		insert_mips_node(name, define_count * (-4));
	}
}


//***************************

void load(char* reg, OperandPtr op) {
	char name[20];
	if(op->kind == VARIABLE){
		sprintf(name, "v%d", op->u.var_no);
	}
	else if(op->kind == TEMP){
		sprintf(name, "t%d", op->u.var_no);
	}
	else if(op->kind == ADDRESS){
		sprintf(name, "t%d", op->u.var_no);
	}
	else if(op->kind == ADDRESS_GET){
		sprintf(name, "v%d", op->u.var_no);
	};

	if (op->kind == CONSTANT) {
		fprintf(fout, "  li %s, %d\n", reg, op->u.value);
	}else if (op->kind == ADDRESS_GET){ //***********************************
		MIPSnode *van = get_mips_node(name);
		fprintf(fout, "  addi %s, $fp, %d \n", reg, van->addr);
	}
	else {
		MIPSnode *van = get_mips_node(name);
#ifdef DEBUG
	//	printf("\nname:%s addr: %d\n", name, van->addr);
#endif
		fprintf(fout, "  lw %s %d($fp) \n", reg, van->addr);
	}
}

void store(char* reg, Operand *op) 
{
	char name[20];
	if(op->kind == VARIABLE){
		sprintf(name, "v%d", op->u.var_no);
	}
	else if(op->kind == TEMP){
		sprintf(name, "t%d", op->u.var_no);
	}
	else if(op->kind == ADDRESS){
		sprintf(name, "t%d", op->u.var_no);
	}
	else if(op->kind == ADDRESS_GET){
		sprintf(name, "v%d", op->u.var_no);
	};


	if (op->kind == CONSTANT) {

	}else {
		MIPSnode* van = get_mips_node(name);
		fprintf(fout, "  sw %s %d($fp) \n", reg, van->addr);
	}
}

void read_write_code() 
{
    fprintf(fout, ".data\n");
    fprintf(fout, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fout, "_ret: .asciiz \"\\n\"\n");
    fprintf(fout, ".globl main\n");
    fprintf(fout, ".text\n");

    fprintf(fout, "read:\n\tli $v0, 4\t\n");
    fprintf(fout, "la $a0, _prompt\n\tsyscall\n");
    fprintf(fout, "\tli $v0, 5\n\tsyscall\n\tjr $ra\n\n");

    fprintf(fout, "write:\n\tli $v0, 1\n");
    fprintf(fout, "\tsyscall\n\tli $v0, 4\n");
    fprintf(fout, "\tla $a0, _ret\n");
    fprintf(fout, "\tsyscall\n\tmove $v0, $0\n");
    fprintf(fout, "\tjr $ra\n\n");
}
