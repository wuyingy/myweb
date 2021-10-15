#include "stdio.h"

void main()
{
	char buf[]="aabBa*wujingyou#ja1#223";
	char buf_g[100];
	int number;
	sscanf(buf,"%[aB]",buf_g);
	printf("%s\n",buf_g);
	sscanf(buf,"%[a-z]",buf_g);
	printf("%s\n",buf_g);
	sscanf(buf,"%[^d]",buf_g);
	printf("%s\n",buf_g);
	sscanf(buf,"%*[^#]%s",buf_g);
	printf("%s\n",buf_g);
	sscanf(buf,"%*[^*]%*[*]%[^#]",buf_g);
	printf("%s\n",buf_g);
	sscanf(buf,"%*[^#]%*[#]%*[^#]%*[#]%d",&number);
	printf("%d\n",number);
	//sscanf(buf,"");

	//return 0;
}
