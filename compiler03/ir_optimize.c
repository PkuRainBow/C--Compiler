#include "./include/compiler.h"

/* optimization */
/*
 * 详细优化说明见实验报告优化列表分析
 */
void ir_optimize(ICN_Ptr *irhead) 
{
    ICN_Ptr cur_node, prev_node, next_node;
    ICN_Ptr irtail = *irhead;
    InterCodePtr cur_ir, prev_ir, next_ir;
    OperandPtr op1, op2, op3;
    int imm;
    
    if (NULL == irhead || NULL == *irhead)
        return;
    while (NULL != irtail->next) 
        irtail = irtail->next;
    
    cur_node = irtail;

    while (NULL != cur_node) {
        cur_ir = cur_node->code;
        assert(NULL != cur_ir);

		switch (cur_ir->kind) {
            /*
             * optimize the case  like ：
             *  LABLE 1：
             *  LABLE 2：
             */
			case IR_LABEL:
				prev_node = cur_node->prev;
				prev_ir = prev_node->code;
				if(prev_ir->kind == IR_LABEL){
					ICN_Ptr temp_node = *irhead;
					while(temp_node != NULL){
						if(temp_node->code->kind == IR_LABEL
								&& strcmp(temp_node->code->u.uniop.op->u.label_name, 
									prev_ir->u.uniop.op->u.label_name) == 0){
							strcpy(temp_node->code->u.uniop.op->u.label_name,
									cur_node->code->u.uniop.op->u.label_name);
						}
						temp_node = temp_node->next;
					}
					del_inter_code(irhead, prev_ir);
				}
				break;
		    /*
             * optimize the case  like ：
             *  RETURN #1
             *  GOTO label2
             */
			case _GOTO:
				//if the prev is return, needn't goto
				prev_node = cur_node->prev;
				prev_ir = prev_node->code;
				if(prev_ir->kind == _RETURN){
					del_inter_code(irhead, cur_ir);
				}
		    /*
             * optimize the case  like ：
             *  t1 = #2
             *  v1 = t1
             */	
			case _ASSIGN:
				prev_node = cur_node->prev;
				prev_ir = prev_node->code;
				if(prev_ir->kind == _ASSIGN && prev_ir->u.assign.right->kind == CONSTANT){
					OperandPtr temp_constant = (OperandPtr) malloc(sizeof (struct Operand_));
					temp_constant->kind = CONSTANT;
					temp_constant->u.value = prev_ir->u.assign.right->u.value;
					cur_node->code->u.assign.right = temp_constant;
					del_inter_code(irhead, prev_ir);
				}

				break;
			/*
             * optimize the case  like ：
             *  t1 = t3
             *  t2 = t4
             *  t4 = t1+t2
             */	
			case _ADD:
			case _SUB:
			case _DIV:
			case _MUL:
				/* optimize the binary operations by deleting tmps */
				op1 = cur_ir->u.binop.op1;
				op2 = cur_ir->u.binop.op2;
				if (TEMP == op1->kind && TEMP != op2->kind) {
					/* back one step */
					prev_node = cur_node->prev;
					assert(NULL != prev_node);

					prev_ir = prev_node->code;
					if (_ASSIGN == prev_ir->kind) {
						op3 = prev_ir->u.assign.left;
						if (TEMP == op3->kind && op3->u.var_no == op1->u.var_no) {
							cur_ir->u.binop.op1 = prev_ir->u.assign.right;
							del_inter_code(irhead, prev_ir);
						}
					}
				}
				else if (TEMP != op1->kind && TEMP == op2->kind) {
					/* back one step */
					prev_node = cur_node->prev;
					assert(NULL != prev_node);
					prev_ir = prev_node->code;

					if (_ASSIGN == prev_ir->kind) {
						op3 = prev_ir->u.assign.left;
						if (TEMP == op3->kind && op3->u.var_no == op2->u.var_no) {
							cur_ir->u.binop.op2 = prev_ir->u.assign.right;
							del_inter_code(irhead, prev_ir);
						}

					}
				}
				else if (TEMP == op1->kind && TEMP == op2->kind) {
					/* back two step */
					prev_node = *irhead;//cur_node->prev->prev; //*irhead
					assert(NULL != prev_node);
					while (prev_node != cur_node) {
						prev_ir = prev_node->code;
						if (_ASSIGN == prev_ir->kind) {
							op3 = prev_ir->u.assign.left;
							if (op3 != NULL && TEMP == op3->kind) {
								if (op3->u.var_no == op2->u.var_no) {
									cur_ir->u.binop.op2 = prev_ir->u.assign.right;
									del_inter_code(irhead, prev_ir);
								}
								else if (op3->u.var_no == op1->u.var_no) {
									cur_ir->u.binop.op1 = prev_ir->u.assign.right;
									del_inter_code(irhead, prev_ir);
								}
							}
						}
						prev_node = prev_node->next;
					}
				}
				break;
				
			/*
             * optimize the case  like ：
             *  t1 = #2 ADD #3
             *  RETURN t1
             */	
			case _RETURN:
				op1 = cur_ir->u.uniop.op;
				if (TEMP == op1->kind) {
					/* back one step */
					prev_node = cur_node->prev;
					assert(NULL != prev_node);
					prev_ir = prev_node->code;

					switch(prev_ir->kind) {
						case _ASSIGN:
							/* eliminate the redundancy temp variable */
							op2 = prev_ir->u.assign.left;
							if (TEMP == op2->kind && op1->u.var_no == op2->u.var_no) {
								cur_ir->u.uniop.op = prev_ir->u.assign.right;
								del_inter_code(irhead, prev_ir);
							}
							break;
							/* calculate the immediate number */
						case _ADD:
							op2 = prev_ir->u.binop.op1;
							op3 = prev_ir->u.binop.op2;
							if (CONSTANT == op2->kind && CONSTANT == op3->kind) {
								imm = op2->u.value + op3->u.value;
								goto bin_imm_handle;
							}
							break;
						case _SUB:
							op2 = prev_ir->u.binop.op1;
							op3 = prev_ir->u.binop.op2;
							if (CONSTANT == op2->kind && CONSTANT == op3->kind) {
								imm = op2->u.value - op3->u.value;
								goto bin_imm_handle;
							}
							break;
						case _MUL:
							op2 = prev_ir->u.binop.op1;
							op3 = prev_ir->u.binop.op2;
							if (CONSTANT == op2->kind && CONSTANT == op3->kind) {
								imm = op2->u.value * op3->u.value;
								goto bin_imm_handle;
							}
							break;
						case _DIV:
							op2 = prev_ir->u.binop.op1;
							op3 = prev_ir->u.binop.op2;
							if (CONSTANT == op2->kind && CONSTANT == op3->kind) {
								imm = op2->u.value / op3->u.value;
								goto bin_imm_handle;
							}
							break;
bin_imm_handle:
							op2 = prev_ir->u.binop.result;
							if (TEMP == op2->kind && op1->u.var_no == op2->u.var_no) {
								cur_ir->u.uniop.op->kind = CONSTANT;
								cur_ir->u.uniop.op->u.value = imm;
								del_inter_code(irhead, prev_ir);
							}
							break;
						default:
							break;
					}
				}
				break;
			/*
             * optimize the case  like ：
             *  t1 = call func_name
             *  v2 = t1
             */	
			case _CALL:
				next_node = cur_node->next;
				next_ir = next_node->code;
				cur_ir = cur_node->code;
				op1 = cur_ir->u.assign.left;
				op2 = next_ir->u.assign.left;
				op3 = next_ir->u.assign.right;
				if (TEMP == op1->kind && op1->u.var_no == op3->u.var_no) {
					cur_ir->u.assign.left = op2;
					free(op1);
					del_inter_code(irhead, next_ir);
				}
				break;
			case _RELOP:
				/* optimize the relop operations by deleting tmps */
				op1 = cur_ir->u.cond.relop1;
				op2 = cur_ir->u.cond.relop2;
				if (TEMP == op1->kind && TEMP != op2->kind) {
					/* back one step */
					prev_node = cur_node->prev;
					assert(NULL != prev_node);

					prev_ir = prev_node->code;
					if (_ASSIGN == prev_ir->kind) {
						op3 = prev_ir->u.assign.left;
						if (TEMP == op3->kind && op3->u.var_no == op1->u.var_no) {
							cur_ir->u.cond.relop1 = prev_ir->u.assign.right;
							del_inter_code(irhead, prev_ir);
						}
					}
				}
				else if (TEMP != op1->kind && TEMP == op2->kind) {
					/* back one step */
					prev_node = cur_node->prev;
					assert(NULL != prev_node);
					prev_ir = prev_node->code;

					if (_ASSIGN == prev_ir->kind) {
						op3 = prev_ir->u.assign.left;
						if (TEMP == op3->kind && op3->u.var_no == op2->u.var_no) {
							cur_ir->u.cond.relop2 = prev_ir->u.assign.right;
							del_inter_code(irhead, prev_ir);
						}

					}
				}
				else if (TEMP == op1->kind && TEMP == op2->kind) {
					/* back two step */
					prev_node = cur_node->prev->prev;
					assert(NULL != prev_node);

					prev_ir = prev_node->code;
					if (_ASSIGN == prev_ir->kind) {
						op3 = prev_ir->u.assign.left;
						if (TEMP == op3->kind) {
							if (op3->u.var_no == op2->u.var_no) {
								cur_ir->u.cond.relop2 = prev_ir->u.assign.right;
								del_inter_code(irhead, prev_ir);
							}
							else if (op3->u.var_no == op1->u.var_no) {
								cur_ir->u.cond.relop1 = prev_ir->u.assign.right;
								del_inter_code(irhead, prev_ir);
							}
						}
					}
					prev_node = prev_node->next;
					prev_ir = prev_node->code;
					if (_ASSIGN == prev_ir->kind) {
						op3 = prev_ir->u.assign.left;
						if (TEMP == op3->kind) {
							if (op3->u.var_no == op2->u.var_no) {
								cur_ir->u.cond.relop2 = prev_ir->u.assign.right;
								del_inter_code(irhead, prev_ir);
							}
							else if (op3->u.var_no == op1->u.var_no) {
								cur_ir->u.cond.relop1 = prev_ir->u.assign.right;
								del_inter_code(irhead, prev_ir);
							}
						}
					}
				}
				break;
defalut:
				break;
		}
		cur_node = cur_node->prev;
	}
}
