#include "iostream"
#include "stdio.h"
using namespace std;

struct student
{
	char* name;
};
struct teacher
{
	char* name;
	int stu_num;
	struct student **stu_arr;
};
void createMemory(struct teacher*** T,int tn)
{
	struct teacher** t=NULL;
	t=(struct teacher**)calloc(tn,sizeof(struct teacher*));//给变量分配内存，用来存放老师的指针
	int i,j;
	//int sn;
	for(i=0;i<tn;i++)
	{
		//给老师分配内存
		t[i]=(struct teacher*)calloc(1,sizeof(struct teacher));
		printf("Please enter student number for the %d teacher\n",i);
		scanf("%d",&(t[i]->stu_num));
		t[i]->stu_arr=(struct student**)malloc(sizeof(struct student*)*(t[i]->stu_num));
		for(j=0;j<t[i]->stu_num;j++)
		{
			t[i]->stu_arr[j]=(struct student*)calloc(1,sizeof(struct student));
		}
	}
	*T=t;
}
void getValue(struct teacher** T,int tn)
{
        int i;
	int j;
	for(i=0;i<tn;i++)
	{
	      T[i]->name=(char*)calloc(1,10);
	      printf("Please enter name for teacher no.%d\n",i);
	      cin>>T[i]->name;
	      for(j=0;j<T[i]->stu_num;j++)
	      {
	              T[i]->stu_arr[j]->name=(char*)calloc(1,10);
		      printf("Please input name for student no.%d\n",j);
		      cin>>T[i]->stu_arr[j]->name;
	      }
	}
}
void display(struct teacher** T,int tn)
{
	int i,j;
	for(i=0;i<tn;i++)
	{
		printf("The teacher for the %d class is :%s\n",i,T[i]->name);
		printf("This teacher`s student is:\n");
		for(j=0;j<T[i]->stu_num;j++)
		{
			printf("%s\t",T[i]->stu_arr[j]->name);
			cout<<endl;
		}
	}
}
void freeMemory(struct teacher** T,int tn)
{
	int i,j;
	for(i=0;i<tn;i++)
	{
		for(j=0;j<T[i]->stu_num;j++)
		{
			free(T[i]->stu_arr[j]->name);
			free(T[i]->stu_arr[j]);
		}
		free(T[i]->name);
		free(T[i]->stu_arr);
		free(T[i]);
	}
	free(T);
}
int main()
{
	teacher **tea_arr=NULL;
	int tn;
	printf("Please enter the number for teachers:\n");
	scanf("%d",&tn);
	//create
	createMemory(&tea_arr,tn);//Cannot use tea_arr,beacuse it`s NULL
	//get value
	getValue(tea_arr,tn);
	//ouput
	display(tea_arr,tn);
	//free
	freeMemory(tea_arr,tn);

	return 0;
}
