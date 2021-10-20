#include "stdio.h"

struct person
{
	int a;
	char b;
};
struct student
{
	char c;
	int d;
	struct person pp;
};
void main()
{
	struct person p={'d',2,3,'r'};
}
