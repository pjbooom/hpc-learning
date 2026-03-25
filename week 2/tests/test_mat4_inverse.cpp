#include<iostream>
#include<cmath>
#include "math/mat4.h"

int main() {
	Mat4 testMat;
	testMat.m[0] = 4;
	testMat.m[5] = 0;
	testMat.m[15] = 21;
	testMat.m[7] = 22;
	Mat4 testMat_Inv = testMat.inverse();
	Mat4 result = testMat.multiply(testMat_Inv);
	float epsilon = 0.0001f;
	float expectedValue;
		for (int i = 0; i < 16; ++i) {
			if (i == 0 || i == 5 || i == 10 || i == 15) {
				expectedValue = 1.0f;
			}
			else {
				expectedValue = 0.0f;
			}
			if (std::abs(result.m[i] - expectedValue) < epsilon) {
				std::cout << "[PASS] Inverse test passed for element " << i << "has element:" << result.m[i] << std::endl;
			}
			else {
				std::cout << "[FAIL] Inverse test failed for element " << i << ". Expected " << expectedValue << ", got " << result.m[i] << std::endl;
			}
		}
	return 0;
}
