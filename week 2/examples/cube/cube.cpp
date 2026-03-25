//making a FUCKING CUBE COOOOLLLL
#include <iostream>
#include "math/mat4.h"
#include<vector>
#include "math/vec.h"

int main() {
	//A cube has 8 corners (vertices).Let's make a cube that is exactly 2 meters wide, 2 meters tall, and 2 meters deep, and let's place it perfectly dead-center in the universe at Origin (0, 0, 0).Remember, our matrices are $4 \times 4$, which means they can only multiply against a 4D Vector: [X, Y, Z, W].And remember, for raw 3D objects that haven't been pushed through the W-Trap yet, W is always 1.0f.
	std::vector<Vec4> cube = {    //here 'cube' contains the 8 corners of the cube, each corner is a Vec4 with x,y,z,w coordinates. we are using 2.0f for width, height and depth, so the corners will be at -1.0f and 1.0f in x,y,z directions from the center (0,0,0)
		{-1.0f, -1.0f, -1.0f, 1.0f}, // Back-bottom-left corner
		{1.0f, -1.0f, -1.0f, 1.0f},  // Back-bottom-right corner
		{1.0f, 1.0f, -1.0f, 1.0f},   // Back-top-right corner
		{-1.0f, 1.0f, -1.0f, 1.0f},  // Back-top-left corner
		{-1.0f, -1.0f, 1.0f, 1.0f},  // Front-bottom-left corner
		{1.0f, -1.0f, 1.0f, 1.0f},   // Front-bottom-right corner
		{1.0f, 1.0f, 1.0f, 1.0f},    // Front-top-right corner
		{-1.0f, 1.0f, 1.0f, 1.0f}    // Front-top-left corner
	};
	//-------------ENGINE LOGIC------------------
	//Now we need to actually build the Model, View, and Projection matrices using the mat4.h file you just spent all that time perfecting.

	//Let's start incredibly simple.

		//Model: We want the cube to just sit exactly where it is(at the origin, unrotated).We just need a default Identity Matrix.

		//View : We want the camera to just sit at the origin looking forward.We just need another default Identity Matrix.

		//Projection : We need to call your brand new perspective() function!

	Mat4 model; //this is an identity matrix by default, thanks to the constructor we wrote in mat4.h
	Mat4 view;
	Mat4 proj = Mat4::perspective(90.0f, 1.777f, 0.1f, 100.0f); //FOV of 90 degrees, aspect ratio of 16:9 (which is 1.777), near plane of 0.1 meters, and far plane of 100 meters

	//Building MVP PIPELINE, which is multiplying model,view and projection matrix
	//GOLDEN RULE OF MATRIX MULTIPLICATION: Always read the rightmost matrix first, and work your way left. So, we multiply the projection matrix by the view matrix, and then multiply that result by the model matrix. This way, when we eventually multiply our cube's coordinates by the final MVP matrix,
	// they will first be transformed by the model matrix (which does nothing in this case), 
	// then by the view matrix (which also does nothing), and finally by 
	// the projection matrix (which applies perspective).
	Mat4 mvp = proj.multiply(view).multiply(model); //this is the final MVP matrix that we will use to transform our cube's coordinates
	std::cout << "Projecting 3D Cube to 2D Screen Cordinates" << std::endl;
	std::cout << "------------------------------"<<std::endl;
	//Now loop through all 8 cordinates of your cube
	for (int i = 0; i < cube.size(); i++) {
		Vec4 v = cube[i]; //get the current vertex of the cube
		//multiply that vertx with master matrix
		Vec4 transformed = mvp.multiply(v);
		//Since we are not using GPU right now, we will emulate its TRAP w part
		float FinalX = transformed.x / transformed.w; //divide x by w to get perspective division
		float FinalY = transformed.y / transformed.w;
		float FinalZ = transformed.z / transformed.w;
		//printing the final cordinates after it went through MVP pipeline
		// 5. Print the final Normalized Device Coordinates
		std::cout << "Vertex " << i << " 2D Screen Pos: X=" << FinalX << ", Y=" << FinalY << " (Depth Z=" << FinalZ << ")" << std::endl;
	}
}
