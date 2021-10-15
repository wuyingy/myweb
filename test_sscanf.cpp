#include "iostream"
#include "stdio.h"
void test1()
{
	//stop when meet the character specified--^
	//ignore--*
	char buf[]="fuvbd23fj&wujingyou*232";
	char buf_g[100];
	sscanf(buf,"%5s",buf_g);//get 5 characters
	printf("get-5:%s\n",buf_g);
	sscanf(buf,"%[1-9a-z]",buf_g);//get anyone character between 1~9 or between a~z from first address
	printf("1-9a-z:%s\n",buf_g);
	sscanf(buf,"%[^1-9]",buf_g);//symbol ^ represents stop when meet anyone character between 1~9,get from firt,
	printf("^1-9:%s\n",buf_g);//but not include the last charcter tested.
	sscanf(buf,"%*[^&]%*&%s\n",buf_g);//symbol * represents ignore from first until meet &,
}
int main()
{
	test1();
}
