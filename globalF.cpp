#include "iostream"
#include "string.h"

using namespace std;

int sum=100;

void test(int num)
{
	int sum=num;
	::sum+=sum;
	cout<<"sum="<<sum<<endl<<"::sum"<<::sum<<endl;
}

int main()
{
	test(200);
	return 0;
}
