#include "iostream"

int main()
{
	int a=10;
	int &b=a;
	printf("a=%u\n",&a);
	printf("&b=%u\n",&b);

	return 0;
}
