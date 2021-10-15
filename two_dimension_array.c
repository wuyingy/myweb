#include "stdio.h"

void display0(int (*p)[333],int len1,int len2)
{
	int i,j;
	for(i=0;i<len1;i++)
	{
		for(j=0;j<len2;j++)
		{
			printf("%d\t",p[i][j]);
		}
		printf("\n");
	}
}
void display1(int* p,int len1,int len2)
{
	int i,j;
	for(i=0;i<9;i++)
	{
		//for(j=0;j<len2;j++)
		  //      printf("%d\t",*((p+i)+j));
		  printf("%d\n",*p);
		  p+=1;
	}
	//printf("\n");
}
int main()
{
	int arr[][3]={1,2,3,4,5,6,7,8,9};
	display1(arr[0],3,3);
	/*printf("arr=%d\n",arr);
	printf("arr[0][0]=%d\n",&arr[0][0]);
	printf("*arr=%d\n",*arr);
	int i;
	for(i=0;i<9;i++)
		printf("arr[%d]=%u\n",i,arr[i]);
	printf("\n");
	int *p=arr[0];
	for(;p<;i++)
		printf("arr[%d]=%d\n",i,*arr[i]);*/
	return 0;
}
