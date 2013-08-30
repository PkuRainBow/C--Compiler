#include "ir.h"

typedef struct MIPSnode MIPSnode;
struct MIPSnode{
	char var[20];
	int addr;
	MIPSnode *next;
};

extern MIPSnode *mips_addr_list;
extern MIPSnode *get_mips_node(char* var);
extern void output_mips();
extern void print_mips_code(InterCodePtr code);
extern MIPSnode *new_MIPSnode(char *var, int addr);
extern void insert_mips_node(char *var, int addr);
extern void load(char* reg, Operand *op);
extern void store(char* reg, Operand *op);
extern void insert_op(OperandPtr op);
extern void record_alloc(ICN_Ptr icn, ICN_Ptr codes);
extern void stack_push(char* reg);
extern void stack_pop(char* reg);
extern void read_write_code();
