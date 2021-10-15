#include "iostream"

int main()
{
	char str1[]="widvbdbv";
	char str2[100]="dfuvjd\0vfje";
	char str3[]={'d','d','d','\0','b','v'};
	printf("%s\n",str1);
	printf("%s\n",str2);
	printf("%s\n",str3);//ouput until meet '\0' or output over.
        printf("add=%u\n",(int*)str1);	

	return 0;
}
