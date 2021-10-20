#include "iostream"
#include "string.h"
using namespace std;

class person
{
	private:
		char* name=NULL;
		string sex;
		int age;
		const int grade;
		static int numbers;//initialize outside class
	public:
		person(int g):grade(g)
		{
			name=(char*)calloc(1,10);
			if(name!=NULL)
				name="baby";
			sex="female";//girl
			age=1;
			numbers++;
			printf("finish g1.\n");
		}
	/*	person(char* n,string s,int a,int g):sex(s),age(a),grade(g)
	        {
			name=(char*)malloc(10);
			name=n;
			numbers++;
			printf("finish g2.\n");
		}//mainly for grade*/
		void resetName()
		{
			printf("Please enter new name:\n");
			cin>>this->name;
		}
		void resetAge();
		void display() const
		{
			//printf("%s is %d year old.\n",name,age);
			cout<<name<< <<"is"<<age<< <<"year old,"<<"she/he is"<<sex<<endl;
		}
		static int allPerson()
		{
			printf("the number for all person is:%d\n",numbers);
			return numbers;
		}
		~person()
		{
			if(name!=NULL)
			{
			        free(name);
			        name=NULL;
			        printf("free name.\n");
			}
		}
};
void person::resetAge()
{
	printf("Please enter new age:\n");
	cin>>age;
}
int person::numbers=0;
int main()
{
	person p1(1);
	//person p2("jieke","male",20,3);
	return 0;
}
