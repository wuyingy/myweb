#include "stdlib.h"
#include "stdio.h"
void main()
{
	char arr[]="234.34";
	float n;
	sscanf(arr,"%f",&n);
	printf("n=%f\n",n);
	if('+'!='-')
		printf("error\n");
}
