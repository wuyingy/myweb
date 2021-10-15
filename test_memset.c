#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void main()
{
	int arr[]={234,45,23,24,23};
	int i;
	for(i=0;i<sizeof(arr)/sizeof(int);i++)
	{
		printf("%d\n",arr[i]);
	}
	memset(&arr[1],'\0',1);
	for(i=0;i<sizeof(arr)/sizeof(int);i++)
	{
		printf("%d\n",arr[i]);
	}
}
