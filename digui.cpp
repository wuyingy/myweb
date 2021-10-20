#include "iostream"

using namespace std;

void test(char* str)
{
	if(*str=='\0')
		return;
	test(str+1);
	printf("%c\n",*str);
}
int main()
{
	char* str="abcdef";
	test(str);

	return 0;
}
