#include "iostream"
class expl
{
	public:
		int age;
	explicit public:
		expl(int age)
		{
			age=age;
		}
};
int main(int argc,char** argv)
{
	expl e='2';
	return 0;
}
