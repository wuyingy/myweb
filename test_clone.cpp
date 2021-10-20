#include "iostream"
#include "stdio.h"

class box
{
	private:
		int height;
		int width;
		int length;
		int volume;
	public:
		box(int h=10,int w=10,int l=10):height(h),width(w),length(l)
		{}
		void display_volume()
		{
			volume=height*width*length;
			printf("Volume=%d\n",volume);
		}
};
int main()
{
	box cube;
	//printf("cube_this=%d\n",cube.display_volume());
	cube.display_volume();
	box cuboid(20,25);
	//printf("cubiod_this=%d\n",cuboid.display_volume());
	cuboid.display_volume();
	box cuboid1(100,50);
	//printf("cubiod1_this=%d\n",cuboid1.display_volume());
	cuboid1.display_volume();
	cuboid1=cuboid;
	cuboid1.display_volume();
	box cuboid2(cuboid);
	cuboid2.display_volume();

	return 0;
}
