#include "./include/compiler.h"

int label_no = 0;
int tmp_no = 0;

/* create a new label with label index  */
OperandPtr new_label() 
{
    OperandPtr label = (OperandPtr)malloc(sizeof(Operand));
    char *label_name = (char *)malloc(strlen("label")+6);
    if (label_no > 100000) {
        printf("error: too many Labels\n");
        exit(-1);
    }
    sprintf(label_name, "label%d", ++label_no);
    label->kind = OP_LABEL;
    label->u.label_name = label_name;
    return label; 
}

/* translate the structure */
ICN_Ptr translate_Structure(PtrTreeNode Exp, OperandPtr place, FieldList *mem_field){
	PtrTreeNode Exp_child = Exp->child[0];
	PtrTreeNode temp_id = Exp->child[2];
	OperandPtr tmp_op1, tmp_op2, tmp_op3;
	char *id_name, *mem_name;
	SymbolNode sym_node;
	FieldList member;
	InterCodePtr new_ICP;
	ICN_Ptr head = NULL;
	int offset = 0;
	int last = 0;
	
	if(Exp_child->rule == EXP__ID){
		id_name = Exp_child->child[0]->ID;
		sym_node = getName(id_name);
		//get the struct from the structure list
		structType temp_struct = getstructType(sym_node->u.type->u.structure->name);
		
		assert(sym_node != NULL);
		mem_name = temp_id->ID;
		member = temp_struct->tail;
		while(member != NULL){
			if(strcmp(member->name, mem_name)==0){
				break;
				last = 1;
			}
			offset += get_type_size(member->type);
			member = member->tail;
		}
		
		if(member == NULL){
			printf("error: Unknown member of the structure %s\n", id_name);
			exit(-1);
		}
		
		if(mem_field != NULL)
		   *mem_field = member;
		   
		tmp_op1 = new_address(sym_node->var_no);
        tmp_op2 = new_op(CONSTANT, offset);    
        tmp_op3 = new_op(TEMP, place->u.var_no);
        
        new_ICP = new_ir_2(_ADD, tmp_op3, tmp_op1, tmp_op2);
        place->kind = ADDRESS;
        
        add_inter_code(&head, new_ICP);
        return head;	
	}
	else {
		printf("error: CAN'T be here\n");
		exit(-1);
	}
}

/* translate the Exp*/
ICN_Ptr translate_Exp(PtrTreeNode Exp,  OperandPtr place){
	  /*
	   * define the var that will be used
	   */
	  //about inter code 
	  InterCodePtr new_ICP;
	  ICN_Ptr head = NULL;
	  ICN_Ptr temp = NULL;
	  IrKind  biOptype;
	  
	  assert(strncmp(Exp->token, "Exp", 20)==0);
	  
	  switch(Exp->rule){
		  case EXP__INT://
		  {
			  int value = Exp->child[0]->int_value;
			  //create a CONSTANT Operand
			  OperandPtr temp_op;
			  temp_op = new_op(CONSTANT, value);
			  //create intercode 
			  new_ICP = new_ir_1(_ASSIGN, place, temp_op);
			  add_inter_code(&head, new_ICP);
			  //place->kind = CONSTANT;
			  //place->u.value = value;
			  return head;		  
		  }
		  
		  case EXP__ID:
		  {
			  char name[32];
			  strncpy(name, Exp->child[0]->ID, 32);			  	  
		      assert(findName(name)!=0);
		      int value = getName(name)->var_no;
		      OperandPtr tmp_op = new_op(VARIABLE, value);
		      place->kind = VARIABLE;
		      place->u.var_no = value;	      
			 // new_ICP = new_ir_1(_ASSIGN, place, tmp_op);
		//	  add_inter_code(&head, new_ICP);
		      return head;      	  
		  }
		  
		  case EXP__EXP_ASSIGNOP_EXP:
		  {
			  /*
			   * left is ID
			   */
			  if(EXP__ID == Exp->child[0]->rule){
				  OperandPtr tmp_op = new_op(TEMP, ++tmp_no);
				  char *name = Exp->child[0]->child[0]->ID;
				  
				  assert(findName(name)!=0);
				  int value = getName(name)->var_no;
				  /* generate code1*/
				  head = translate_Exp(Exp->child[2], tmp_op);
				  /*
				   * debug
				   */				  
				  
				  /* creat a temp var to store the EXP */
				  OperandPtr tmp_op1 = new_op(VARIABLE, value);
				  //create the 
				  new_ICP = new_ir_1(_ASSIGN, tmp_op1, tmp_op);
				  add_inter_code(&head, new_ICP);
				  
				  new_ICP = new_ir_1(_ASSIGN, place, tmp_op1);
				  add_inter_code(&head, new_ICP);
                  return head;		  
			  }
			  else{
				  
				  OperandPtr tmp_op1 = new_op(TEMP, ++tmp_no);
				  
				  head = translate_Exp(Exp->child[0], tmp_op1);
				  OperandPtr tmp_op2 = new_op(TEMP, ++tmp_no);
				  temp = translate_Exp(Exp->child[2], tmp_op2);
				  head = bind_inter_code(head, temp);


				  // new a[i] = a[1];
				  OperandPtr tmp_op3 = new_op(TEMP, ++tmp_no);
				  new_ICP = new_ir_1(_ASSIGN, tmp_op3, tmp_op2);
				  add_inter_code(&head, new_ICP);
				  new_ICP = new_ir_1(_ASSIGN, tmp_op1, tmp_op3);
				  add_inter_code(&head, new_ICP);
				  new_ICP = new_ir_1(_ASSIGN, place, tmp_op1);
				  add_inter_code(&head, new_ICP);
				  return head;
			  }
		  }
		  
		  case EXP__EXP_AND_EXP:
		  case EXP__EXP_OR_EXP:		  
		  case EXP__EXP_RELOP_EXP:
		  case EXP__NOT_EXP:
		  {
			  //creat 2 labels
			  OperandPtr label1 = new_label();
			  OperandPtr label2 = new_label();
			  //create code
			  OperandPtr tmp_op1 = new_op(CONSTANT, 0);
			  new_ICP = new_ir_1(_ASSIGN, place, tmp_op1);
			  add_inter_code(&head, new_ICP);
			  //generate the condition code 
			  temp = translate_Cond(Exp, label1, label2);
			  head = bind_inter_code(head, temp);
			  //add code to the head
			  new_ICP = new_ir_0(IR_LABEL, label1);
			  add_inter_code(&head, new_ICP);
			  OperandPtr tmp_op2 = new_op(CONSTANT, 1);
			  new_ICP = new_ir_1(_ASSIGN, place, tmp_op2);
			  add_inter_code(&head, new_ICP);
			  new_ICP = new_ir_0(IR_LABEL, label2);
			  add_inter_code(&head, new_ICP);
			  return head;
		  }
		  
		  case EXP__EXP_PLUS_EXP:
				biOptype = _ADD;
				goto EXP_Calculate;
		  case EXP__EXP_MINUS_EXP:
				biOptype = _SUB;
				goto EXP_Calculate;
		  case EXP__EXP_STAR_EXP:
				biOptype = _MUL;
				goto EXP_Calculate;
		  case EXP__EXP_DIV_EXP:
				biOptype = _DIV;
				
EXP_Calculate:
		 {
			//use TEMP
			OperandPtr tmp_op1 = new_op(TEMP, ++tmp_no);
			OperandPtr tmp_op2 = new_op(TEMP, ++tmp_no);
			//generate code
			head = translate_Exp(Exp->child[0], tmp_op1);
			//generate code
			temp = translate_Exp(Exp->child[2], tmp_op2);
			head = bind_inter_code(head, temp);


			OperandPtr tmp_op3 = new_op(TEMP, ++tmp_no);
			OperandPtr tmp_op4 = new_op(TEMP, ++tmp_no);
			new_ICP = new_ir_1(_ASSIGN, tmp_op3, tmp_op1);
			add_inter_code(&head, new_ICP);
			new_ICP = new_ir_1(_ASSIGN, tmp_op4, tmp_op2);
			add_inter_code(&head, new_ICP);
			new_ICP = new_ir_2(biOptype, place, tmp_op3, tmp_op4);
			add_inter_code(&head, new_ICP);
			return head;
		}
		 
		 case EXP__LP_EXP_RP:
		 {
			  return translate_Exp(Exp->child[1], place);
		 }
		 
		 case EXP__MINUS_EXP:
		 {
			  /*  translate code 1 */
			  OperandPtr tmp_op1 = new_op(TEMP, ++tmp_no);
			  OperandPtr tmp_op3 = new_op(TEMP, ++tmp_no);
			  head = translate_Exp(Exp->child[1], tmp_op1);
			  new_ICP = new_ir_1(_ASSIGN, tmp_op3, tmp_op1);
			  add_inter_code(&head, new_ICP);
			  /*  translate code 2 */
			  OperandPtr tmp_op2 = new_op(CONSTANT, 0);
			  new_ICP = new_ir_2(_SUB, place, tmp_op2, tmp_op3);
			  add_inter_code(&head, new_ICP);
			  return head;
		 }
		 
		 /*
		  * function without args
		  */
		 case EXP__ID_LP_RP:
		 {
			 char *func_name = Exp->child[0]->ID;
			 if(strcmp(func_name, "read") == 0){
				 if(place == NULL){
					 OperandPtr tmp_op1 = new_op(TEMP, ++tmp_no);
					 new_ICP = new_ir_0(_READ, tmp_op1);
				 }
				 else{
					 new_ICP = new_ir_0(_READ, place);
				 }
				 add_inter_code(&head, new_ICP);
			 }
			 else{
				 OperandPtr tmp_op1 = new_function(func_name);
				 new_ICP = new_ir_1(_CALL, place, tmp_op1);
				 add_inter_code(&head, new_ICP);
			 }
			 return head;		 
		 }
		 
		 //function with args
		 case EXP__ID_LP_ARGS_RP: 
		 {
			 char *func_name = Exp->child[0]->ID;
			 OperandPtr *args;
			 args = (OperandPtr *)malloc(sizeof(OperandPtr)*20);
			 int arg_num = 0;
			 head = translate_Args(Exp->child[2], args, &arg_num);
			 //generate the code 
			 if(strcmp(func_name, "write") == 0){
				 OperandPtr tmp_op = new_op(TEMP, ++tmp_no);
				 if(ADDRESS == args[0]->kind){
					 new_ICP = new_ir_1(_ASSIGN, tmp_op, args[0]);
					 add_inter_code(&head, new_ICP);
				 }
				 else{
					 tmp_op = args[0];
				 }
				 new_ICP = new_ir_0(_WRITE, tmp_op);
				 add_inter_code(&head, new_ICP);
			 }
			 
			 else{
				 int i;
				 for(i=arg_num-1; i>=0; i--){
					 if(args[i]->kind == ADDRESS){
						 OperandPtr tmp_op = new_op(TEMP, ++tmp_no);
						 new_ICP = new_ir_1(_ASSIGN, tmp_op, args[i]);
						 add_inter_code(&head, new_ICP);
						 args[i] = tmp_op;
					 }
				 }
				 for(i=arg_num-1; i>=0; i--){
					 new_ICP = new_ir_0(_ARG, args[i]);
					 add_inter_code(&head, new_ICP);
				 }
				 OperandPtr tmp_op1 = new_function(func_name);
				 new_ICP = new_ir_1(_CALL, place, tmp_op1);
				 add_inter_code(&head, new_ICP);
			 }
			 free(args);
			 return head;
		 }
		 
		 //array
		 case EXP__EXP_LB_EXP_RB:
		 {
			 PtrTreeNode tmp_node1 = Exp->child[0];
			 PtrTreeNode tmp_node2 = Exp->child[2];
			 OperandPtr tmp_op4;
			 char *name;
			 SymbolNode symbol;
			 
			 if (tmp_node1->rule == EXP__ID ||
                  tmp_node1->rule == EXP__EXP_DOT_ID) {				 
				 if(tmp_node1->rule == EXP__ID){
					 name = tmp_node1->child[0]->ID;
					 symbol = getName(name);
					 assert(symbol != 0);
		             int id = symbol->var_no;
		             if(check_param(head, id) == NULL){
						 tmp_op4 = new_address(id);
					 }
					 else{
						 tmp_op4 = check_param(head, id);
					 }
				 }
				 else{
					 tmp_op4 = new_op(TEMP, ++tmp_no);
					 head = translate_Structure(tmp_node1, tmp_op4, &(symbol->u.type->u.structure));
					 tmp_op4->kind = TEMP;
				 }
				 
				 OperandPtr tmp_op1 = new_op(TEMP, ++tmp_no);
				 temp = translate_Exp(tmp_node2, tmp_op1);//
//				 OperandPtr tmp_oop1 = new_op(TEMP, ++tmp_no);//
//				 head = bind_inter_code(head, temp);
//				 temp = translate_Exp(tmp_node2, tmp_oop1);
				 head = bind_inter_code(head, temp);
//				 new_ICP = new_ir_1(_ASSIGN, place, tmp_op1);
//				 add_inter_code(&head, new_ICP);
				 //get the offset of the array
				 int offset = 1;
				 
				 Type array_type = symbol->u.type->u.array.elem;
				 Type g_array_d = symbol->u.type->u.array.elem;
				 g_array_d = g_array_d->u.array.elem;
				 
				 while(array_type != NULL && array == array_type->kind){
					 offset *= array_type->u.array.size;
					 array_type = array_type->u.array.elem;
				 }

                 OperandPtr tmp_op2 = new_op(CONSTANT, offset * 4); 
                 OperandPtr tmp_op3 = new_op(TEMP, ++tmp_no);				 
				 new_ICP = new_ir_2(_MUL, tmp_op3, tmp_op1, tmp_op2);
				 add_inter_code(&head, new_ICP);
				 
				 if(g_array_d==NULL){
					 /*
					  *fix the bug 
					  */
					 tmp_op2 = new_op(TEMP, place->u.var_no);
					 new_ICP = new_ir_2(_ADD, tmp_op2, tmp_op4, tmp_op3);
					 //tmp_op2->kind = ADDRESS;
					 //InterCodePtr temp_ICP = new_ir_1(_ASSIGN, place, tmp_op2);
					 place->kind = ADDRESS;
					// add_inter_code(&head, new_ICP);
					 //add_inter_code(&head, temp_ICP);
					 //return head;
				 }
				 else{
					 new_ICP = new_ir_2(_ADD, place, tmp_op4, tmp_op3);
				 }
				 add_inter_code(&head, new_ICP);
				 return head;
			 }
		 }
			 
		 case EXP__EXP_DOT_ID:
			 return translate_Structure(Exp, place, NULL);
			 
		 default:
		 {
			 printf("error: Mysterious EXP: %s at line: %d\n", Exp->token, Exp->lineno);
			 exit(-1);
		 }
	 }
	 return NULL;           
}

/* translate the Stmt*/
ICN_Ptr translate_Stmt(PtrTreeNode stmt)
{
	OperandPtr temp_op1, temp_op2, label1, label2, label3;
	ICN_Ptr code1 = NULL, code2 = NULL;
	InterCodePtr temp_code;
	switch(stmt->rule){
		case STMT__EXP_SEMI:
			if(stmt->child[0]->rule == EXP__ID_LP_ARGS_RP
					|| stmt->child[0]->rule == EXP__ID_LP_RP){
				temp_op1 = new_op(TEMP, ++tmp_no);
				code1 = translate_Exp(stmt->child[0], temp_op1);
				return code1;
			}
			return translate_Exp(stmt->child[0], NULL);
		case STMT__COMPST:
			return translate_Compst(stmt->child[0]);
		case STMT__RETURN_EXP_SEMI:
			temp_op1 = new_op(TEMP, ++tmp_no);
			temp_op2 = new_op(TEMP, ++tmp_no);
			code1 = translate_Exp(stmt->child[1], temp_op1);
			temp_code = new_ir_1(_ASSIGN, temp_op2, temp_op1);
			add_inter_code(&code1, temp_code);
			temp_code = new_ir_0(_RETURN, temp_op2);
			add_inter_code(&code1, temp_code);
			return code1;
		case STMT__IF_LP_EXP_RP_STMT:
			label1 = new_label();
			label2 = new_label();

			code1 = translate_Cond(stmt->child[2], label1, label2);
			code2 = translate_Stmt(stmt->child[4]);

			temp_code = new_ir_0(IR_LABEL, label1);
			add_inter_code(&code1, temp_code);
			code1 = bind_inter_code(code1, code2);
			temp_code = new_ir_0(IR_LABEL, label2);
			add_inter_code(&code1, temp_code);
			return code1;
		case STMT__IF_LP_EXP_RP_STMT_ELSE_STMT:
			label1 = new_label();
			label2 = new_label();
			label3 = new_label();

			code1 = translate_Cond(stmt->child[2], label1, label2);
			temp_code = new_ir_0(IR_LABEL, label1);
			add_inter_code(&code1, temp_code);

			code2 = translate_Stmt(stmt->child[4]);
			code1 = bind_inter_code(code1, code2);
			
			temp_code = new_ir_0(_GOTO, label3);
			add_inter_code(&code1, temp_code);

			/* create [LABEL label2] */
			temp_code = new_ir_0(IR_LABEL, label2);
			add_inter_code(&code1, temp_code);

			/* generate code 3(else part) */
			code2 = translate_Stmt(stmt->child[6]);
			code1 = bind_inter_code(code1, code2);

			/* create [LABEL label3] */
			temp_code = new_ir_0(IR_LABEL, label3);
			add_inter_code(&code1, temp_code);

			return code1;
		case STMT__WHILE_LP_EXP_RP_STMT:
			label1 = new_label();
			label2 = new_label();
			label3 = new_label();

			temp_code = new_ir_0(IR_LABEL, label1);
			add_inter_code(&code1, temp_code);

			// generate condition code
			code2 = translate_Cond(stmt->child[2], label2, label3);
			code1 = bind_inter_code(code1, code2);
			// create LABEL 
			temp_code = new_ir_0(IR_LABEL, label2);
			add_inter_code(&code1, temp_code);
			//generate the loop block
			code2 = translate_Stmt(stmt->child[4]);
			code1 = bind_inter_code(code1, code2);
			//create [GOTO label1] 
			temp_code = new_ir_0(_GOTO, label1);
			add_inter_code(&code1, temp_code);
			//create [LABEL label3]
			temp_code = new_ir_0(IR_LABEL, label3);
			add_inter_code(&code1, temp_code);
			return code1;
		default:
			printf("error: Unkonwn Statement \n");
			exit(-1);
	}
}

/* translate the Compst*/
ICN_Ptr translate_Compst(PtrTreeNode compst){
	ICN_Ptr code1 = NULL, code2 = NULL;
	assert(compst != NULL);
	// be careful here, DefList or StmtList may be NULL
	if(strncmp(compst->child[1]->token,"DefList", 20) == 0
			&& strncmp(compst->child[2]->token, "StmtList", 20) == 0){
		code1 = translate_DefList(compst->child[1]);
		code2 = translate_StmtList(compst->child[2]);
		code1 = bind_inter_code(code1, code2);
	}
	else if(strncmp(compst->child[1]->token, "DefList", 20) == 0
			&& strncmp(compst->child[2]->token, "RC", 20) == 0){
		code1 = translate_DefList(compst->child[1]);
	}
	else if(strncmp(compst->child[1]->token, "StmtList", 20) == 0){
		code1 = translate_StmtList(compst->child[1]);
	}

	return code1;
}

/* translate the StmtList*/
ICN_Ptr translate_StmtList(PtrTreeNode stmtlist){
	ICN_Ptr code1 = NULL, code2 = NULL;

	if (NULL == stmtlist)
		return NULL;

	if(stmtlist->rule == STMTLIST__STMT_STMTLIST){
		code1 = translate_Stmt(stmtlist->child[0]);
		code2 = translate_StmtList(stmtlist->child[1]);
		code1 = bind_inter_code(code1, code2);
		return code1;
	}
	else {
		printf("error: Unknown StmtList\n");
		exit(-1);
	}
}

/* translate the Cond */
ICN_Ptr translate_Cond(PtrTreeNode Exp, OperandPtr l_true, OperandPtr l_false){
	unsigned int rule = Exp->rule;
	OperandPtr tmp_op1, tmp_op2, tmp_op3, tmp_op4;
	ICN_Ptr head = NULL;
	ICN_Ptr temp = NULL;
	InterCodePtr new_ICP;
	RelopKind relop;
	OperandPtr label;
	
	switch(rule){
		case EXP__EXP_RELOP_EXP:
		{
			/*
			* optimize the  case CONSTANT
			*/		
			tmp_op1 = new_op(TEMP, ++tmp_no);
			tmp_op2 = new_op(TEMP, ++tmp_no);
			/* generate code 1, the left part of the relop */
			head = translate_Exp(Exp->child[0], tmp_op1);
			/* generate code 2, the right part of the relop */
			temp = translate_Exp(Exp->child[2], tmp_op2);


			/* create code 3 */
			relop = get_relop_type(Exp->child[1]);
//			new_ICP = new_ir_cond(relop, tmp_op1, tmp_op2, l_true);
			head = bind_inter_code(head, temp);

			tmp_op3 = new_op(TEMP, ++tmp_no);
			tmp_op4 = new_op(TEMP, ++tmp_no);
			new_ICP = new_ir_1(_ASSIGN, tmp_op3, tmp_op1);
			add_inter_code(&head, new_ICP);
			new_ICP = new_ir_1(_ASSIGN, tmp_op4, tmp_op2);
			add_inter_code(&head, new_ICP);
			new_ICP = new_ir_cond(relop, tmp_op3, tmp_op4, l_true);

			add_inter_code(&head, new_ICP);
			/* create [GOTO label_false] */
			new_ICP = new_ir_0(_GOTO, l_false);
			add_inter_code(&head, new_ICP);
			return head;
		}

		case EXP__NOT_EXP:
		{
			return translate_Cond(Exp->child[1], l_false, l_true);
		}

		case EXP__EXP_AND_EXP:
		{
			label = new_label();
			//generate code 1
			head = translate_Cond(Exp->child[0], label, l_false);
			//generate code 2
			temp = translate_Cond(Exp->child[2], l_true, l_false);
			new_ICP = new_ir_0(IR_LABEL, label);
			add_inter_code(&head, new_ICP);
			head = bind_inter_code(head, temp);
			return head;
		}

		case EXP__EXP_OR_EXP:
		{
			label = new_label();
			head = translate_Cond(Exp->child[0], l_true, label);
			temp = translate_Cond(Exp->child[2], l_true, l_false);
			new_ICP = new_ir_0(IR_LABEL, label);
			add_inter_code(&head, new_ICP);
			head = bind_inter_code(head, temp);
			return head;
		}

		default:
		{
			tmp_op1 = new_op(TEMP, ++tmp_no);
			tmp_op3 = new_op(TEMP, ++tmp_no);
			head = translate_Exp(Exp, tmp_op1);
			new_ICP = new_ir_1(_ASSIGN, tmp_op3, tmp_op1);
			add_inter_code(&head, new_ICP);
			tmp_op2 = new_op(CONSTANT, 0);
			new_ICP = new_ir_cond(RELOP_NE, tmp_op3, tmp_op2, l_true);
			add_inter_code(&head, new_ICP);
			new_ICP = new_ir_0(_GOTO, l_false);
			add_inter_code(&head, new_ICP);
			return head;
		}	
	}
}

/* translate the Args*/
ICN_Ptr translate_Args(PtrTreeNode Args, OperandPtr *arglist, int *arg_num){
	char *name;
	SymbolNode temp_node;
	unsigned int rule = Args->rule;
	OperandPtr tmp_op1, tmp_op2;
	ICN_Ptr head = NULL;
	ICN_Ptr temp = NULL;
	InterCodePtr new_ICP;
	PtrTreeNode Exp;

	switch(rule){
		case ARGS__EXP:
			{
				Exp = Args->child[0];
				if(Exp->rule==EXP__ID){
					name = Exp->child[0]->ID;
					temp_node = getName(name);
					assert(temp_node != NULL);

					if(temp_node->kind == structure){
						tmp_op1 = new_address(temp_node->var_no);
						tmp_op2 = new_op(TEMP, ++tmp_no);
						new_ICP = new_ir_1(_ASSIGN, tmp_op2, tmp_op1);
						add_inter_code(&head, new_ICP);
						arglist[(*arg_num)++] = tmp_op2;
						return head;
					}
					else{
						tmp_op1 = new_op(TEMP, ++tmp_no);
						head = translate_Exp(Args->child[0], tmp_op1);
						arglist[(*arg_num)++] = tmp_op1;
					}
				}

				else{
					tmp_op1 = new_op(TEMP, ++tmp_no);
					head = translate_Exp(Args->child[0], tmp_op1);
					arglist[(*arg_num)++] = tmp_op1;
				}
				return head;
			}

		case ARGS__EXP_COMMA_ARGS:
			{
				Exp = Args->child[0];

				if(Exp->rule == EXP__ID){
					name = Exp->child[0]->ID;
					temp_node = getName(name);
					assert(temp_node != NULL);
					if(temp_node->u.type->kind == array || temp_node->u.type->kind == structure)
					{
						tmp_op1 = new_address(temp_node->var_no);
						tmp_op2 = new_op(TEMP, ++tmp_no);
						new_ICP = new_ir_1(_ASSIGN, tmp_op2, tmp_op1);
						add_inter_code(&head, new_ICP);
						arglist[(*arg_num)++] = tmp_op2;
					}
					else{
						tmp_op1 = new_op(TEMP, ++tmp_no);
						head = translate_Exp(Args->child[0], tmp_op1);
						arglist[(*arg_num)++] = tmp_op1;
					}
				}

				else{
					tmp_op1 = new_op(TEMP, ++tmp_no);
					head = translate_Exp(Args->child[0], tmp_op1);
					arglist[(*arg_num)++] = tmp_op1;
				}

				temp = translate_Args(Args->child[2], arglist, arg_num);
				head = bind_inter_code(head, temp);
				return head;
			}

		default:
			{
				printf("error: Mysterious Args ...\n");
				exit(1);
			}
	}	
}

/* translate the ExtDef*/
ICN_Ptr translate_ExtDef(PtrTreeNode extdef) {
	ICN_Ptr code1 = NULL, code2 = NULL;

	switch(extdef->rule) {
		case EXTDEF__SPECIFIER_EXTDECLIST_SEMI:
			return translate_ExtDecList(extdef->child[1]);
		case EXTDEF__SPECIFIER_SEMI:
			break;
		case EXTDEF__SPECIFIER_FUNDEC_COMPST:
			code1 =  translate_FunDec(extdef->child[1]);
			code2 = translate_Compst(extdef->child[2]);
			code1 = bind_inter_code(code1, code2);
			return code1;
	}
	return NULL;
}

/* translate the ExtDecList */
ICN_Ptr translate_ExtDecList(PtrTreeNode extdeclist){
	ICN_Ptr code1 = NULL, code2 = NULL;
	InterCodePtr temp_code;
	if(extdeclist->rule == EXTDECLIST__VARDEC){
		return translate_VarDec(extdeclist->child[0], NULL);
	}
	else if (extdeclist->rule == EXTDECLIST__VARDEC_COMMA_EXTDECLIST) {
		code1 = translate_VarDec(extdeclist->child[0], NULL);
		code2 = translate_ExtDecList(extdeclist->child[2]);
		code1 = bind_inter_code(code1, code2);
		return code1;
	}
}

/* translate the variable declaration */
ICN_Ptr translate_VarDec(PtrTreeNode vardec, OperandPtr *pvar) {
	unsigned int rule = vardec->rule;
	ICN_Ptr code1 = NULL, code2 = NULL;
	OperandPtr tmp_op1, tmp_op2;
	InterCodePtr temp_code;
	char *var_name;
	SymbolNode var;
	int size;
	if (VARDEC__ID == rule) {
		var_name = vardec->child[0]->ID;
		var = getName(var_name);
		/*
		 * basic type
		 */
		if (basic == var->u.type->kind) {
			tmp_op1 = new_op(VARIABLE, var->var_no);
			if (NULL != pvar)
				*pvar = tmp_op1;
			return NULL;
		}
		/*
		 * struture type 
		 */
		else if (structure == var->u.type->kind) {
			/*
			 * fix a bug about the structure
			 */
			Type temp_type = (Type)malloc(sizeof(struct Type_));
			/* get the struct type
			 * the symbol_table node store the structure's name
			 * "var->u.type->u.structure->name" stores the struct name
			 * 
			 */
			structType temp_struct = getstructType(var->u.type->u.structure->name);
			temp_type->kind = structure;
			temp_type->u.structure = temp_struct->tail;
			size = get_type_size(temp_type);
			tmp_op1 = new_op(VARIABLE, var->var_no);
			if (NULL != pvar)
				*pvar = tmp_op1;
			tmp_op2 = new_op(CONSTANT, size);
			temp_code = new_ir_1(VAR_DEC, tmp_op1, tmp_op2);
			add_inter_code(&code1, temp_code);
			return code1;
		}
		/*
		 * array type
		 */
		else if (array == var->u.type->kind) {
			size = get_type_size(var->u.type);
			tmp_op1 = new_op(VARIABLE, var->var_no);
			if (NULL != pvar)
				*pvar = tmp_op1;
			tmp_op2 = new_op(CONSTANT, size);
			temp_code = new_ir_1(VAR_DEC, tmp_op1, tmp_op2);
			add_inter_code(&code1, temp_code);
			return code1;	
		}
	}
	/*
	 * multi-array ... WARNING !
	 */
	else if (VARDEC__VARDEC_LB_INT_RB) {
		/* array variable dec */
		return translate_VarDec(vardec->child[0], NULL);
	}
}

/* translate the FunDec */
ICN_Ptr translate_FunDec(PtrTreeNode fundec){
	unsigned int rule = fundec->rule;
	char *fun_name = fundec->child[0]->ID;
	SymbolNode func;
	FieldList param;
	char *bfun_name = (char *)malloc(strlen("func") + 20);
	InterCodePtr new_intercode;
	OperandPtr tmp_op1, tmp_op2, tmp_op3, tmp_op4;

	ICN_Ptr code1 = NULL, code2 = NULL;


	/* get the symbol node of the function */
	sprintf(bfun_name, "func:%s", fun_name);
	func = getName(fun_name);
	assert(NULL != func);
	free(bfun_name);

	tmp_op1 = new_function(fun_name);
	new_intercode = new_ir_0(DEF_FUNCTION, tmp_op1);
	add_inter_code(&code1, new_intercode);

	if (FUNDEC__ID_LP_VARLIST_RP == rule) {
		param = func->u.func.paramTypeList;
		while (NULL != param) {
			tmp_op2 = new_param(param->name); 
			new_intercode = new_ir_0(IR_PARAM, tmp_op2);
			add_inter_code(&code1, new_intercode);
			param = param->tail;
		}
		return code1;
	}
	else if (FUNDEC__ID_LP_RP == rule) {
		return code1;
	}
	else {
		printf("error Unknown FunDec\n");
		exit(-1);
	}
}

/* translate DefList */
ICN_Ptr translate_DefList(PtrTreeNode deflist){
	ICN_Ptr code1 = NULL, code2 = NULL;

	if (NULL == deflist)
		return NULL;

	code1 = translate_Def(deflist->child[0]);
	code2 = translate_DefList(deflist->child[1]);
	code1 = bind_inter_code(code1, code2);
	return code1;
}

/* translate the Def */
ICN_Ptr translate_Def(PtrTreeNode def){
	assert(NULL != def);
	return translate_DecList(def->child[1]);
}

/* translate the DecList */
ICN_Ptr translate_DecList(PtrTreeNode declist){
	unsigned int rule = declist->rule;
	ICN_Ptr code1 = NULL, code2 = NULL;

	if (DECLIST__DEC == rule) {
		return translate_Dec(declist->child[0]);
	}
	else if (DECLIST__DEC_COMMA_DECLIST == rule) {
		code1 = translate_Dec(declist->child[0]);
		code2 = translate_DecList(declist->child[2]);
		code1 = bind_inter_code(code1, code2);
		return code1;
	}
}

/* translate the Dec */
ICN_Ptr translate_Dec(PtrTreeNode dec){
	unsigned int rule = dec->rule;
	ICN_Ptr code1 = NULL, code2 = NULL;
	OperandPtr tmp_op1, tmp_op2;
	InterCodePtr new_intercode;

	if (DEC__VARDEC == rule) {
		return translate_VarDec(dec->child[0], NULL);
	}
	else if (DEC__VARDEC_ASSIGNOP_EXP == rule) {
		/* declaration code */
		// tmp_op1 = new_op(OP_TEMP, --tmp_var_id);
		code1 = translate_VarDec(dec->child[0], &tmp_op1);
		/* generate the expresstion */
		tmp_op2 = new_op(TEMP, ++tmp_no);
		code2 = translate_Exp(dec->child[2], tmp_op2);
		code1 = bind_inter_code(code1, code2);

		new_intercode = new_ir_1(_ASSIGN, tmp_op1, tmp_op2);
		add_inter_code(&code1, new_intercode);
		return code1;
	}
}

