#include "stdio.h"

void main()
{
	int a=10;
	int* p=&a;
	int **pp=&p;
	printf("一级指针大小：%d\n",sizeof(p));
	printf("二级指针大小：%d\n",sizeof(p));
}
