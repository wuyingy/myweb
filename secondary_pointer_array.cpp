#include "iostream"
#include "stdio.h"
#include "stdlib.h"

using namespace std;

struct person
{
	char *name;
	int age;
};
struct person** createSec_pointer(int len)
{
	struct person** per=NULL;
	per=(struct person**)malloc(sizeof(struct person*)*len);
	for(int i=0;i<len;i++)
	{
		per[i]=(struct person*)calloc(1,sizeof(struct person));
	}

	return per;
}
void getData(struct person** per,int pn)
{
	int i;
	for(i=0;i<pn;i++)
	{
		per[i]->name=(char*)calloc(1,10);
		printf("Please input name for the %d person:\n",i);
		cin>>per[i]->name;
		printf("Please input age for the %d person:\n",i);
		scanf("%d",&(per[i]->age));
	}
}
void showPerson(struct person** per,int pn)
{
	int i;
	for(i=0;i<pn;i++)
	{
		printf("%s is %d year old.\n",per[i]->name,per[i]->age);
	}
}
void freePerson(struct person** per,int pn)
{
	int i;
	for(i=0;i<pn;i++)
	{
		if(per[i]->name!=NULL)
		        free(per[i]->name);
		if(per[i]!=NULL)
			free(per[i]);
	}
	free(per);
}
int main()
{
	struct person** per=NULL;
	//create
	int person_n;
	printf("Please input person number:\n");
	scanf("%d",&person_n);
	per=createSec_pointer(person_n);
	//creat data
	getData(per,person_n);	
	//show
	showPerson(per,person_n);
	//release
	freePerson(per,person_n);
	return 0;
}
