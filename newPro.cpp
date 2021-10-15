#include "iostream"
using namespace std;
int* get_value(int* len)
{	
	int* pp=new int[3];
	for(int i=0;i<3;i++)
		cin>>pp[i];
	*len=3;
	return pp;
}
int main()
{
	int *pointer=NULL;
	//int *len=NULL;空指针和野指针不能利用
	int len;
	pointer=get_value(&len);
	printf("len=%d\n",len);
	for(int i=0;i<len;i++)
		printf("%d=%d\n",i,pointer[i]);
	//printf("sizeof(int)=%d\n",sizeof(int));

	delete []pointer;
	pointer=NULL;
	return 0;
}
