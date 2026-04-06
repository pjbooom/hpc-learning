//AOS is needed for image cause we want RGB in one place when CPU searches
#include<iostream>
#include "math/mat4.h"
#include<vector>
#include "math/vec.h"
#include "math/camera.h"
#include<fstream>
#include<limits> //for baycentric method(Z-buffer)


using color = Vec3; //so here Vec3 is renamed as color

/// Sutherland Hodgman Clipping Algorithm
std::vector<Vec3> Clipping(const std::vector<Vec3>& polygon, int axis, float boundary, bool isGreaterBoundary) {
	// Create a new vector called newPolygon, which is defualt emoty, so if polygon is empty then return this polygon
	std::vector<Vec3> newPolygon;
	if (polygon.empty()) {
		return newPolygon;
	}
	//set previous point to last vertex of polygon to draw the line cause computer only sees points and we need to draw line, read the notes!!
	Vec3 previousPoint = polygon.back();

	//walk the perimeter to draw the line from prev point to current point and check if they are inside or outside the boundary and add the points to newPolygon accordingly
	for (size_t i = 0; i < polygon.size(); ++i) {
		Vec3 currentPoint = polygon[i];

		///check axis
		float previousValue;
		float currentValue;

		if (axis == 0) {
			previousValue = previousPoint.x;
			currentValue = currentPoint.x;
		}
		else if (axis == 1) {
			previousValue = previousPoint.y;
			currentValue = currentPoint.y;
		}
		else {
			previousValue = previousPoint.z;
			currentValue = currentPoint.z;
		}
		///check boundary
		bool previousSafe;
		bool currentSafe;
		if (isGreaterBoundary == true) {
			previousSafe = (previousValue <= boundary);
			currentSafe = (currentValue <= boundary);
		}
		else {
			previousSafe = (previousValue >= boundary);
			currentSafe = (currentValue >= boundary);
		}
		///check cases 
		if (previousSafe && currentSafe) {
			//both points are inside boundary, so we add both to the polygon
			newPolygon.push_back(currentPoint);
		}
		else if (previousSafe && !currentSafe) {
			float t = (boundary - previousValue) / (currentValue - previousValue); //calculating the t value for interpolation
			Vec3 intersection = Vec3::lerp(previousPoint, currentPoint, t);
			newPolygon.push_back(intersection);
		}
		else if (!previousSafe && currentSafe) {
			float t = (boundary - previousValue) / (currentValue - previousValue); //calculating the t value for interpolation
			Vec3 intersection = Vec3::lerp(previousPoint, currentPoint, t);
			newPolygon.push_back(intersection);
			newPolygon.push_back(currentPoint);
		}
		else {
		}
		///then for next loop put prevpoint=currentpount to draw line from current point to next point
		previousPoint = currentPoint;
	}
	return newPolygon;
}

//Machine assembly
std::vector<Vec3> ClipPolygon(std::vector<Vec3> polygon) {
	//right wall
	polygon = Clipping(polygon, 0, 1.0f, true);
	//left wall
	polygon = Clipping(polygon, 0, -1.0f, false);
	//top wall
	polygon = Clipping(polygon, 1, 1.0f, true);
	//bottom wall
	polygon = Clipping(polygon, 1, -1.0f, false);
	//near Plane
	polygon = Clipping(polygon, 2, -1.0f, false);

	return polygon;
}

///Rasterization
//step1: Bresenham's Line Algorithm
void DrawLine(int x0, int y0, int x1, int y1, std::vector<color>& canvas, int width) {

	//see if steep is increasing more on y axis than x axis, if so we need to swap x and y axis to make it less steep and easier to draw cause function we wrote below works only if x axis is increasing more than y axis
	bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	//we want to always draw from left to right, so if x0 is greater than x1 we swap the points
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = std::abs(x1 - x0);
	int dy = std::abs(y1 - y0);
	//set D initally
	int D = 2 * dy - dx;
	int y = y0;
	//If y0 is smaller than y1, we step forward (+1). If it's bigger, we step backward (-1).
	int y_step = (y0 < y1) ? 1 : -1;
	//for loop to draw line from x0 to x1
	for (int x = x0; x <= x1; ++x) {
		//C++ is ruthlessly strict about brackets { }. When you write int index inside the if statement, that variable is born, lives, and dies entirely inside those brackets. By the time the CPU reaches your bounds check, index has been wiped from memory. The compiler will scream "Undeclared Identifier!"
		// so define index first here
		int index = 0;
		//get the index of the pixel in canvas cause RAM stores it in 1D we will use our beloved memoery layout formula to get the index
		if (steep) {
			index = x * width + y; //if we swapped x and y axis then we need to swap them back to get the correct index
		}
		else {
			index = y * width + x;
		}
		//check if index is within the boundery and is greater than 0 to avoid accessing out of bound memory
		if (index >= 0 && index < canvas.size()) {
			canvas[index] = color(1.0f, 1.0f, 1.0f); //setting it to white
		}
		//check if D is greater than 0 to decide whether to move up in y axis or not
		if (D >= 0) {
			y = y + y_step;
			D = D - 2 * dx; //if we move up in y axis we need to subtract 2*dx from D cause we are moving diagonally and we need to account for that in our decision variable (error=error-1)
		}
		//increase D 
		D = D + 2 * dy;
	}
}

/// Edge function
float EdgeFunction(const Vec3& A, const Vec3& B, const Vec3& P) {
	return (P.x - A.x) * (B.y - A.y) - (P.y - A.y) * (B.x - A.x);
}

/// Checking if pixel exists withint the traingle, then if it is then colour that pixel
void DrawTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& c0, const Vec3 c1, const Vec3& c2, std::vector<float>& zBuffer, std::vector<Vec3>& canvas, int height, int width) {
	//bounding box
	int minX = std::max(0, (int)std::min(v0.x, std::min(v1.x, v2.x)));
	int maxX = std::max(0, (int)std::max(v0.x, std::max(v1.x, v2.x)));
	int minY = std::min(width-1, (int)std::min(v0.y, std::min(v1.y, v2.y)));
	int maxY = std::min(height-1, (int)std::max(v0.y, std::max(v1.y, v2.y)));

	for (int y = minY; y <= maxY; ++y) {
		for (int x = minX; x <= maxX; ++x) {
			Vec3 P(x + 0.5f, y + 0.5f, 0.0f); //we add 0.5f to get to centre of pixel
			//edge function to check if point is inside the triangle
			float w0 = EdgeFunction(v1, v2, P);
			float w1 = EdgeFunction(v2, v0, P);
			float w2 = EdgeFunction(v0, v1, P);
			//if all weights are greater than 0 then the point is inside the triangle
			if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
				int index = y * width + x;
				//find area of total triangle
				float area = EdgeFunction(v0, v1, v2);
				//find depth of z value of pixel using baycentric method
				//make sure area is not 0, otherwise we will divide by 0 and get NaN
				if (area == 0) {
					continue;
				}
				//normalise the weights by dividing them by the area of the triangle as we have seen(refer to ipad notes)
				float w0_norm = w0 / area;
				float w1_norm = w1 / area;
				float w2_norm = w2 / area;
				//interpolate the z value of the pixel using the normalised weights and the z values of the vertices
				float pixel_z = (w0_norm * v0.z) + (w1_norm * v1.z) + (w2_norm * v2.z);
				//check between Z buffer and pixel z value to see if we need to update the pixel colour
				if (pixel_z < zBuffer[index]) {
					zBuffer[index] = pixel_z; //update z buffer with new depth value
					//invetrning colours of mixed RGB values using bacymetirc color mixing, where we stole XYZ of triangle in cube
					float mixed_R = (w0_norm * c0.x) + (w1_norm * c1.x) + (w2_norm + c2.x);
					float mixed_G = (w0_norm * c0.y) + (w1_norm * c1.y) + (w2_norm + c2.y);
					float mixed_B = (w0_norm * c0.z) + (w1_norm * c1.z) + (w2_norm + c2.z);
					canvas[index] = color(mixed_R, mixed_G, mixed_B);
				}
			}
		}
	}
}

int main() {

	//Image settings
	int width = 800;
	int height = 600;

	//AOS called canvas of structre Vec3
	// Initializes width*ht pixels with black color (0,0,0)
	std::vector<color> canvas(width * height, color(0.0f, 0.0f, 0.0f));   //here we have canvas AOS with 480000 vectors called pixels and eash vector is 0.0f,0.0f,0.0f which is defualt black color
	std::vector<float> zBuffer(width * height, std::numeric_limits<float>::max());

	///position of camera in world space
	Vec3 eye(3.0f, 3.0f, 3.0f); //camera is 5 meters back and 2 meters up from origin
	Vec3 target(0.0f, 0.0f, 0.0f); //camera is looking at orgin
	Vec3 up(0.0f, 1.0f, 0.0f); //up is exactly Y Axis
	//generate matrices
	Mat4 model; //model is identity matrix, since we want cube to be at origin and unrotated
	Mat4 view = lookAt(eye, target, up);
	Mat4 proj = Mat4::perspective(75.0f, (float)width / (float)height, 0.01f, 100.0f);
	//master matrix
	Mat4 MVP = proj.multiply(view.multiply(model)); //since we dont have any model matrix, we can directly multiply projection and view matrix to get VP matrix which we will use to transform our cube's coordinates

	// 1. Define the 12 triangles of the cube!
	// Each triangle is just a list of 3 Vec3 points.
	std::vector<std::vector<Vec3>> cube_triangles = {
		// Front face
		{Vec3(-1,-1, 1), Vec3(1,-1, 1), Vec3(1, 1, 1)},
		{Vec3(-1,-1, 1), Vec3(1, 1, 1), Vec3(-1, 1, 1)},
		// Back face
		{Vec3(1,-1,-1), Vec3(-1,-1,-1), Vec3(-1, 1,-1)},
		{Vec3(1,-1,-1), Vec3(-1, 1,-1), Vec3(1, 1,-1)},
		// Left face
		{Vec3(-1,-1,-1), Vec3(-1,-1, 1), Vec3(-1, 1, 1)},
		{Vec3(-1,-1,-1), Vec3(-1, 1, 1), Vec3(-1, 1,-1)},
		// Right face
		{Vec3(1,-1, 1), Vec3(1,-1,-1), Vec3(1, 1,-1)},
		{Vec3(1,-1, 1), Vec3(1, 1,-1), Vec3(1, 1, 1)},
		// Top face
		{Vec3(-1, 1, 1), Vec3(1, 1, 1), Vec3(1, 1,-1)},
		{Vec3(-1, 1, 1), Vec3(1, 1,-1), Vec3(-1, 1,-1)},
		// Bottom face
		{Vec3(-1,-1,-1), Vec3(1,-1,-1), Vec3(1,-1, 1)},
		{Vec3(-1,-1,-1), Vec3(1,-1, 1), Vec3(-1,-1, 1)}
	};
	// 1. The Master Loop: Put the first triangle on the conveyor belt.
	for (int i = 0; i < cube_triangles.size(); i++) {

		// 2. THE BUCKET: We create an empty, temporary list for THIS specific triangle.
		// We must collect all 3 transformed points before we can move to the next stage.
		std::vector<Vec3> projected_triangle;

		// 3. Process the 3 corners of the current triangle
		for (int j = 0; j < 3; j++) {

			// Grab the raw 3D coordinate (e.g., -1, 1, 1)
			Vec3 original_point = cube_triangles[i][j];

			// 4. Hit it with the MVP Matrix!
			// Because your custom math library automatically includes the perspective divide,
			// this single line translates the point, rotates it, applies the camera lens, 
			// AND shrinks it down into Normalized Device Coordinates (-1.0 to 1.0).
			Vec3 projected_point = MVP.mult_4dmat_vec3(original_point);

			// 5. Drop the finished, flattened point into our temporary bucket
			projected_triangle.push_back(projected_point);
		}
		//Cliiping
		std::vector<Vec3> clipped_polygon = ClipPolygon(projected_triangle);
		//check if traingle survived the clipping process, if not then skip to next triangle
		// SAFETY CHECK: Did the triangle survive?
	// If the triangle was completely behind the camera, the clipper destroyed it.
	// If it has less than 3 points, it's not a printable shape anymore. Skip it!
		if (clipped_polygon.size() < 3) {
			continue;
		}
		//VEIWPORT TRANFORM
		//create vector that stores after viewport
		std::vector<Vec3> screen_vertices;
		for (int j = 0; j < clipped_polygon.size(); ++j) {
			Vec3 v = clipped_polygon[j];
			float pixelX = ((v.x + 1.0f) * 0.5f * width); //// Stretch the X axis: Math (-1 to 1) -> Screen (0 to width)
			//// Stretch the Y axis: Math (-1 to 1) -> Screen (height to 0)
        // We do (1.0f - v.y) because pixel Y=0 is the TOP of the screen!
			float pixelY = ((1.0f - v.y) * 0.5f * height); //we do 1.0f - because in screen space y is inverted, so we need to flip it backint pixelX = (int)((screenPos.x + 1.0f) * 0.5f * width);
			//drop this to screen vertices
			screen_vertices.push_back(Vec3(pixelX, pixelY, v.z)); //we also keep the z value for depth sorting later on
		}

		// --- STAGE 5: SOLID RASTERIZATION ---
	// We use a for-loop to create the Triangle Fan. 
	// We start j at 1, and stop before the very last point.
		for (int j = 1; j < screen_vertices.size() - 1; ++j) {
			Vec3 anchor = screen_vertices[0];
			Vec3 current = screen_vertices[j];
			Vec3 next = screen_vertices[j + 1];
			//now defining colors using XYZ of projected triangle
			color c0 = color((projected_triangle[0].x + 1.0f) * 0.5f, (projected_triangle[0].y + 1.0f) * 0.5f, (projected_triangle[0].z + 1.0f) * 0.5f);
			color c1 = color((projected_triangle[j].x + 1.0f) * 0.5f, (projected_triangle[j].y + 1.0f) * 0.5f, (projected_triangle[j].z + 1.0f) * 0.5f);
			color c2 = color((projected_triangle[j+1].x + 1.0f) * 0.5f, (projected_triangle[j+1].y + 1.0f) * 0.5f, (projected_triangle[j+1].z + 1.0f) * 0.5f);
			// Draw this triangle!
			DrawTriangle(anchor, current, next, c0, c1, c2, zBuffer, canvas, height, width);
		}
		
	}

	///FILE WRITER
	//std::ofstream: This stands for Output File Stream. It is a C++ tool that asks the Operating System (Windows/Mac/Linux) for permission to create and write to a file.

	//("clipped_scene.ppm") : You are handing the OS the name of the file.If the file doesn't exist, the OS creates a blank one in the exact same folder where your code is running. If a file with that name already exists, the OS completely deletes its contents and overwrites it.
	std::ofstream outFile("clipped_scene.ppm");

	//PPM header
	//"P3\n": This is called a "Magic Number" in file formats. P3 specifically tells the image viewer: "Hey, this is a plain-text file, and every 3 numbers you read equal one RGB pixel." The \n hits the "Enter" key to start a new line.

	//width << " " << height << "\n": We print the engine's resolution (e.g., 800 600) so the image viewer knows exactly when to wrap the pixels around to the next row.

		//"255\n" : This tells the viewer the Maximum Color Value.It says, "If you see the number 255, that means maximum brightness. If you see 0, it means pitch black."
	outFile << "P3\n" << width << " " << height << "\n255\n";
	//Your canvas is a flat, 1D array of 480,000 pixels. Because we already handled the 2D math (the Viewport Transform) up in Step B, we don't need a complex nested for loop here. We just start at pixel 0 and blindly march forward to pixel 479,999, grabbing whatever color is stored there.
	for ( int i = 0; i < width * height; ++i) {
		//Digital to Analog conversion
		// canvas[i].r: This pulls the red float value out of your RAM(e.g., 1.0f).

			//* 255.99f : This scales your perfect engine math(0.0 to 1.0) up into the 8 - bit color space(0.0 to 255.99) that monitors use.The .99 prevents floating - point rounding errors from artificially darkening your pure colors.

			//(int) : This permanently chops off the decimals.It turns 255.99 into exactly 255, safely packing it into an integer that the.ppm file expects.
		int ir = (int)(255.99f * canvas[i].r);
		int ig = (int)(255.99f * canvas[i].g);
		int ib = (int)(255.99f * canvas[i].b);

		//Finally, we write the three integers for this pixel's RGB color into the file, separated by spaces. The PPM format expects every pixel to be written as three integers in a row, so we follow that format exactly.
		outFile << ir << " " << ig << " " << ib << "\n";
	}
	//This is arguably the most important line in the block. When you use << to send text to a file, C++ doesn't write it to the hard drive immediately. Hard drives are slow, so C++ holds the text in a temporary RAM "buffer" until it has a large chunk to write all at once.
	//Calling file.close() forces C++ to flush any remaining text out of the buffer, save the file permanently to the disk, and politely give control of the file back to the Operating System.If your engine crashes before hitting this line, your image file will be corrupt and half - empty.
	outFile.close();
	std::cout << "Engine successfully rendered frame to clipped_scene.ppm!" << std::endl;

	return 0;
}

