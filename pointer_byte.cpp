#include "iostream"

int main()
{
	printf("int--byte=%d\n",sizeof(int));
	printf("char--byte=%d\n",sizeof(char));
	printf("float--byte=%d\n",sizeof(float));
	printf("double--byte=%d\n",sizeof(double));
	printf("bool--byte=%d\n",sizeof(bool));

	int a=9;
	char b='b';
	float f=12.3;
	double d=23.342323;
	bool g=true;

	printf("p--int-b=%d\n",sizeof(&a));
	printf("p--char-b=%d\n",sizeof(&b));
	printf("p--float-b=%d\n",sizeof(&f));
	printf("p--double-b=%d\n",sizeof(&d));
	printf("p--bool-b=%d\n",sizeof(&g));

	return 0;
}
