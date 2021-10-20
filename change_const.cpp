#include "iostream"
using namespace std;

const int a=10;
void test()
{
	const int b=10;
	int *p=(int*)&b;
	*p=20;
	printf("b=%d\n",b);
}
int main()
{
	//int *p=(int*)&a;
	//*p=20;
	//printf("a=%d\n",a);
	test();

	return 0;
}
