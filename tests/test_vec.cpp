#include<iostream>
#include "math/vec.h"
int main() {
	std::cout << "------Running vector MATH TESTS-------" << std::endl;
	Vec3 right(1.0f, 0.0f, 0.0f);
	Vec3 up(0.0f, 1.0f, 0.0f);
	if (right.dot(up) == 0.0f) {
		std::cout << "[PASS] Dot product test passed!" << std::endl;
	}
	else {
		std::cout << "[FAIL] Dot product test failed!" << std::endl;
	}
	Vec3 cross_product = right.cross(up);
	if (cross_product.x == 0.0f && cross_product.y == 0.0f && cross_product.z == 1.0f) {
		std::cout << "[PASS] Cross product test passed!" << std::endl;
	}
	else {
		std::cout << "[FAIL] Cross product test failed!" << std::endl;
	}
	Vec3 rand_vec(3.0f, 4.0f, 0.0f);
	Vec3 norm_vec = rand_vec.normalize();
	float epsilon = 0.0001f;
	float length = norm_vec.length();
	if (std::abs(length - 1.0f) < epsilon) {
		std::cout << "[PASS] Normalusation test passed!" << std::endl;
	}
	else {
		std::cout << "[FAIL] Normalisation test failed!" << std::endl;
	}
	return 0;
}