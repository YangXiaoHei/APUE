#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define YH_MAGIC_NUMBER 7362359

#define YH_PASTE(_content_, _suffix_)  (_content_##_suffix_)

#define YH_VAR(_var_, __random__) YH_PASTE(_var_, __random__)

#if  !defined(SWAP) && !defined(__SWAP__INTERNAL) 
#define __SWAP__INTERNAL(_x_, _y_, __random__) 					\
do {															\
	typeof(_x_) YH_VAR(__a__, __random__) = _x_;				\
	typeof(_x_) YH_VAR(__b__, __random__) = _y_;				\
	typeof(_x_) YH_VAR(__tmp__, __random__) = _x_;				\
	YH_VAR(__tmp__, __random__) = a;							\
	YH_VAR(__a__, __random__) = YH_VAR(__b__, __random__);		\
	YH_VAR(__b__, __random__) = YH_VAR(__tmp__, __random__);  	\
	_x_ = YH_VAR(__a__, __random__);						  	\
	_y_ = YH_VAR(__b__, __random__);							\
} while (0);
#define SWAP(_x_, _y_) __SWAP__INTERNAL(_x_, _y_, YH_MAGIC_NUMBER)
#endif

int main(int argc, char *argv[]) {

	double a = 3.14, b = 9.54;

	SWAP(a, b);

	printf("a = %f b = %f\n", a, b);

	return 0;	
}