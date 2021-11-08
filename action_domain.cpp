#include "iostream"
#include "string.h"
using namespace std;

namespace xiaoming
{
	void disp_name(string name)
	{
		cout<<name<<endl;
	}
	void disp_age(int age)
	{
		cout<<"xiaoming is "<<age<<endl;
	}
}
namespace xiaobao
{
	void disp_name(string name)
	{
		cout<<name<<endl;
	}
	void disp_age(int age)
	{
		cout<<"xiaobao is "<<age<<endl;
	}
}
int main()
{
	xiaoming::disp_name("xiaoming");
	::disp_age(22);

	return 0;
}
