#include <iostream>
#include "math/vec.h"      //preprocessor
int main() {
	Vec3 right(1.0f, 0.0f, 0.0f);
	Vec3 up(0.0f, 1.0f, 0.0f);
	std::cout << "Right vector:" << right << std::endl;
	std::cout << "Up vector:" << up << std::endl;
	//dot product of x axis and y axis should be 0
	std::cout << "Dot product of right and up (Should be 0):" << right.dot(up) << std::endl;
	//cross product of x axis and y axis should be z axis
	std::cout << "Corss product of right and up(should be [0,0,1]):" << right.cross(up) << std::endl;
	//length of right vector should be 1
	std::cout << "length of right vector(should be 1):" << right.length() << std::endl;
	//normalised right vector should be same as right vector, cause its already normalised
	std::cout << "Normalised right vector(should be same as right vector):" << right.normalize() << std::endl;
	return 0;
}
