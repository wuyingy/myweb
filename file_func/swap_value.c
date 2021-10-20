#include "stdio.h"

void main()
{
	char a='a';
	char b='b';
	char c;
	c=a^b;
	a=c^a;
	b=c^b;
	printf("a=%c\tb=%c\n",a,b);
}
