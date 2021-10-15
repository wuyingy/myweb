#include "iostream"
#include "string.h"
using namespace std;

void func(char* p)
{
	int len=sizeof(p);//Attention:the byte of pointer is 8bite
	printf("len2=%d\n",len);
	int lenc=strlen(p);
	printf("lenc=%d\n",lenc);
}
int main()
{
	char arr[]="dfvdfvggrwt";
	int len=sizeof(arr);
	printf("len1=%d\n",len);
	func(arr);

	return 0;
}
