#include "iostream"
#include "stdio.h"

struct person
{
	char c;
	double d;
	char h;
};
struct data_test
{
	char a;
	short b;
	char c[2];
	double d;
	struct person p3;
	char k;
	int i;
	char o;
	struct person pp;
}data;
int main()
{
	/*int len;
	len=sizeof(person);
	if(len==12)
		printf("Linux对齐系数为：4\n");
	if(len==16)
		printf("Linux对齐系数为：8\n");
	else printf("error\n");*/

	printf("size--person=%d\n",sizeof(person));
	printf("size--data=%d\n",sizeof(data));

	return 0;
}
