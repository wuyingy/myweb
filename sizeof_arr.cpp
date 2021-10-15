#include "iostream"

int main()
{
	int arr[]={13,3,123,134,13,13};
	printf("sizeof(arr)=%d\n",sizeof(arr));
	printf("&arr[0]=%d\n",&arr[0]);
	printf("arr=%d\n",arr);
	printf("&arr=%d\n",&arr);
	printf("arr+1=%d\n",arr+1);
	printf("&arr+1=%d\n",&arr+1);

	return 0;
}
