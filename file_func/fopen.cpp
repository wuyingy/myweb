#include "iostream"
#include "stdio.h"

FILE* getFILE()
{
	FILE* gfile;
	gfile=fopen("./text.c","r");
	return gfile; 
}

void read_data(FILE* file)
{
	int len=0;
	char buf[1024];
	while(fgets(buf,1024,file)!=NULL)
	{
		len++;
		printf("%s",buf);
	}
}

int main()
{
	FILE* file;
	file=getFILE();
	read_data(file);

	return 0;
}
