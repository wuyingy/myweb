#include "stdio.h"

int main()
{
	char *name=NULL;
	name=malloc(10);
	//char *p=name;
	scanf("%s",name);
	printf("%s\n",name);
	scanf("%s\n",name);
	printf("%s\n",name);
	free(name);

	return 0;
}
