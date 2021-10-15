#include "iostream"
namespace A
{
	int a;
	int b=10;
}
namespace A
{
	int c;
}
int n=10;
int main()
{
	int n=20;
	printf("局部n=%d\n",n);
	printf("全局n=%d\n",::n);
	return 0;
}
