#include "iostream"
#include "stdio.h"
#include "string.h"
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
                friend student* max_score(student s[],int n);
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
student* max_score(student s[],int n)
{
	int i;
	student *p=s;
	int gID,cache;
	//sprintf(&gID,"%s",s[0].stuID);
	gID=stoi(s[0].stuID);
	for(i=1;i<n;i++)
	{
		//sprintf(&cache,"%s",s[i].stuID);
		cache=stoi(s[i].stuID);
		if(cache>gID)
			p=&s[i];
	}
	cout<<p->name<<" is a "<<p->sex<<", her/his score is "<<p->score<<" id is "<<p->stuID<<endl;
	return p;
}
int main()
{
	student stu[5]={student("lixiaoli","2020111",1980,"female"),
	                student("wuji","2020112",201,"male"),
	                student("meimei","2020113",198,"female"),
	                student("zhangxiaojia","2020114",199,"male"),
	                student("hemiao","2020115",200,"male")};
	int i;
	/*for(i=0;i<5;i++)
	{
	        display1(&stu[i]);
	}*/
	//cout<<"classScore="<<student::classScore<<endl;
	max_score(stu,5);

	return 0;
}
