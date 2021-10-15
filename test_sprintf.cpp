#include "iostream"
#include "stdio.h"
#include "string.h"
using namespace std;

int main()
{
	int num=123343;
	char buf[100];
	sprintf(buf,"%d",num);
	//printf("%s\n",buf);
	int len=strlen(buf);
	for(int i=0;i<len;i++)
		printf("%d=%c\n",i,buf[i]);

	return 0;
}
