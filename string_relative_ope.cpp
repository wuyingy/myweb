#include "iostream"
#include "string.h"
using namespace std;

int main()
{
	string str1="vdufbvf",s1="yhhhhuu";
	string str2="iwufvbdfv",s2="Yhhhuu";

	if(str1>str2)
		cout<<"str1>str2"<<endl;
	else if(str1<str2)
		cout<<"str1<str2"<<endl;

	if(s1>s2)
		cout<<"s1 "<<s1<<endl;
	else if(s1<s2)
		cout<<"s2 "<<s2<<endl;

	int t=str1.compare(2,4,str2);
	cout<<"t="<<t<<endl;

	char arr[]="iwudfbc";
	int g=str1.compare(arr);
	cout<<"g="<<g<<endl;

	return 0;
}
