#include "iostream"
#include "stdio.h"

using namespace std;

class student 
{
	private:
		string name;
		string stuID;
		float score;
		const string sex;
	public:
		student(string na,string id,float sc,string se):name(na),stuID(id),score(sc),sex(se)//4 arguments
	        {
			classScore+=sc;
		}
		static float classScore;
		friend void display(student &);
		friend void display1(student *);
		~student()
		{
			classScore=0;
			cout<<"clear classScore"<<endl;
		}
};
float student::classScore=0;
//template <typename T>
void display(student &s)
{
	cout<<s.name<<" is a "<<s.sex<<", her/his score is "<<s.score<<" id is "<<s.stuID<<endl;
}
void display1(student *s)
{
	cout<<s->name<<" is a "<<s->sex<<", her/his score is "<<s->score<<" id is "<<s->stuID<<endl;
}
int main()
{
	student stu[5]={student("lixiaoli","2020111",1980,"female"),
	                student("wuji","2020112",201,"male"),
	                student("meimei","2020113",198,"female"),
	                student("zhangxiaojia","2020114",199,"male"),
	                student("hemiao","2020115",200,"male")};
	int i;
	for(i=0;i<5;i++)
	{
	        display1(&stu[i]);
	}
	cout<<"classScore="<<student::classScore<<endl;
	return 0;
}
