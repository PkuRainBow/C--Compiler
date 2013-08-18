struct Operands
{
 int o1;
 int o2;
 int p;
};

int main()
{
	struct Operands op;
	op.o1 = 1;
	op.o2 = 2;
	return op.p;
}
