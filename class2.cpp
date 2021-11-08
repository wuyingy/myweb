#include "iostream"
#include "string.h"
using namespace std;

class person
{
	public:
		int age;
		string name
	public:
		person(int ag,string st):age(ag),name(st)
	        {}
		void dis_age()
		{
			cout<<age<<endl;
		}
		void dis_name()
		{
			cout<<name<<endl;
		}
};
class person2
{
	public:
		int age;
		string name
	public:
		person2(int ag,string st):age(ag),name(st)
	        {}
		void dis_age()
		{
			cout<<age<<endl;
		}
		void dis_name()
		{
			cout<<name<<endl;
		}
};
int main(int argc,char** argv)
{
	person p1(22,"meili");
	person2 p2(23,"taimeili");

	

	return 0;
}
