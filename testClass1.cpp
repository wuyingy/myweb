#include "iostream"
#include "string.h"
using namespace std;

class person
{
	private:
		string name;
		string sex;
		int age;
		const int grade;
		float height;
		static int numbers;//initialize outside class
	public:
		person(int g):grade(g)
		{
			name="baby";
			sex="female";//girl
			age=1;
			numbers++;
		}
		person(string n,string s,int a,int g,float h,float w):sex(s),age(a),grade(g),height(h),weight(w)
	        {
			name=n;
			numbers++;
		}//mainly for grade

		float weight;

		friend void display_figure(person p);
		void resetName()
		{
			printf("Please enter new name:\n");
			cin>>name;
		}
		void resetAge();
		void display() const
		{
			//printf("%s is %d year old,she/he is %s\n",name,age,sex);
			cout<<name<<" "<<"is"<<age<<" "<<"year old,"<<"she/he is"<<sex<<endl;
		}
		static int allPerson()
		{
			printf("the number for all person is:%d\n",numbers);
			return numbers;
		}
		~person()
		{
			numbers=0;
		}
};
void person::resetAge()
{
	printf("Please enter new age:\n");
	cin>>age;
}
int person::numbers=0;

void display_figure(person p)
{
	cout<<p.name<<" is "<<p.height<<" cm "<<"and "<<p.weight<<" kg"<<endl;//kilometers
}

int main()
{
	person p1(1);
	p1.display();
	p1.resetName();
	p1.display();
	p1.allPerson();
	person p2("jieke","male",20,3,170.3,120.5);
	p2.display();
	p2.resetAge();
	p2.display();
	person::allPerson();
	display_figure(p2);
	//p2.display();

	return 0;
}
