#include "stdio.h"

int func(int a,int b)
{
	return a+b;
}
void main()
{
	typedef int (*typeFunc)(int a,int b);
	typeFunc pp=func;
	printf("%d\n",pp(4,5));
}
