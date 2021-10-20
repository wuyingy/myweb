#include "stdio.h"
#include "iostream"

int main()
{
	char *name=NULL;
	name=new char[10];
	char *p=name;
	scanf("%s",name);
	printf("%s\n",name);
	scanf("%s\n",name);
	printf("%s\n",name);
	delete p;

	return 0;
}
