#include "iostream"
#include "cstring"
struct student
{
	char name[10];
	int age;
};
struct person
{
	char *name;
	int age;
};
int main()
{
	student s1={"laosan",14};
	student s2={"laoda",10};
	printf("%s\tis\t%d\n",s1.name,s1.age);
	printf("%s\tis\t%d\n",s2.name,s2.age);
	s1=s2;
	printf("%s\tis\t%d\n",s1.name,s1.age);
	printf("%s\tis\t%d\n",s2.name,s2.age);

	//need to create memory before suing for pointer
	person p1;
	p1.name=new char(10);
	strcpy(p1.name,"zhangmazi");
	p1.age=33;
	printf("%s\tis\t%d\n",p1.name,p1.age);
	
	person p2;
	p2.name=new char(10);
	strcpy(p2.name,"wangmazi");
	p2.age=40;
	printf("%s\tis\t%d\n",p2.name,p2.age);

	p1=p2;
	printf("p1=p2\n");
	printf("%s\tis\t%d\n",p1.name,p1.age);
	printf("%s\tis\t%d\n",p2.name,p2.age);

	if(p1.name!=NULL)
	{
		delete p1.name;
		p1.name=NULL;
	}
	if(p2.name!=NULL)
	{
		delete p2.name;
		p2.name=NULL;
	}
	printf("%s\tis\t%d\n",p2.name,p2.age);

	return 0;
}
