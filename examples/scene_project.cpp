#include<iostream>
#include "math/mat4.h"
#include<vector>
#include "math/vec.h"
#include "math/camera.h"
#include<fstream>
#include<limits> //for baycentric method(Z-buffer)
#include <algorithm> //we neeed this for max and min
#include <chrono>


using color = Vec3; //so here Vec3 is renamed as color

struct Vertex {
	Vec3 position;
	Vec3 normal;
	float u;
	float v;
};

//defining what a traingle is 
struct Triangle {
	Vertex v0;
	Vertex v1;
	Vertex v2;
};

//creating a struct for SOA
struct Mesh_SoA {
	std::vector<Vec3> positions;
	std::vector<Vec3> normals;
	std::vector<float> u;
	std::vector<float> v;
};

//defining texture
struct Texture {
	int width;
	int height;
	std::vector<color> pixels;
};


/// Sutherland Hodgman Clipping Algorithm
std::vector<Vertex> Clipping(const std::vector<Vertex>& polygon, int axis, float boundary, bool isGreaterBoundary) {
	// Create a new vector called newPolygon, which is defualt emoty, so if polygon is empty then return this polygon
	std::vector<Vertex> newPolygon;
	if (polygon.empty()) {
		return newPolygon;
	}
	//set previous point to last vertex of polygon to draw the line cause computer only sees points and we need to draw line, read the notes!!
	Vertex previousVertex = polygon.back();

	//walk the perimeter to draw the line from prev point to current point and check if they are inside or outside the boundary and add the points to newPolygon accordingly
	for (size_t i = 0; i < polygon.size(); ++i) {
		Vertex currentVertex = polygon[i];

		///check axis
		float previousValue;
		float currentValue;

		if (axis == 0) {
			previousValue = previousVertex.position.x;
			currentValue = currentVertex.position.x;
		}
		else if (axis == 1) {
			previousValue = previousVertex.position.y;
			currentValue = currentVertex.position.y;
		}
		else {
			previousValue = previousVertex.position.z;
			currentValue = currentVertex.position.z;
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
			newPolygon.push_back(currentVertex);
		}
		else if (previousSafe && !currentSafe) {
			float t = (boundary - previousValue) / (currentValue - previousValue); //calculating the t value for interpolation
			Vertex intersection;
			intersection.position = Vec3::lerp(previousVertex.position, currentVertex.position, t);
			intersection.normal = Vec3::lerp(previousVertex.normal, currentVertex.normal, t);
			newPolygon.push_back(intersection);
		}
		else if (!previousSafe && currentSafe) {
			float t = (boundary - previousValue) / (currentValue - previousValue); //calculating the t value for interpolation
			Vertex intersection;
			intersection.position = Vec3::lerp(previousVertex.position, currentVertex.position, t);
			intersection.normal = Vec3::lerp(previousVertex.normal, currentVertex.normal, t);
			newPolygon.push_back(intersection);
		}
		else {
		}
		///then for next loop put prevpoint=currentpount to draw line from current point to next point
		previousVertex = currentVertex;
	}
	return newPolygon;
}

//Machine assembly
std::vector<Vertex> ClipPolygon(std::vector<Vertex> polygon) {
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
//triangulation
std::vector<Triangle> Triangulate(const std::vector<Vertex> polygon) {
	std::vector<Triangle> finalTriangle;
	if (polygon.size() < 3) {
		return finalTriangle;
	}
	for (int i = 1; i < polygon.size()-1; ++i) {
		Triangle newTriangle;
		newTriangle.v0= polygon[0];
		newTriangle.v1= polygon[i];
		newTriangle.v2= polygon[i + 1];
		finalTriangle.push_back(newTriangle);
	}
	return finalTriangle;
}

//Rasterization
void Draw_triangle(const Triangle& pixelTri, int screenWidth, int screenheight, std::vector<float>& zBuffer, std::vector<color>& canvas, const Texture& tex) {
	//finding max and min of x
	if (tex.pixels.empty()) return; // SAFETY CATCH: If texture is empty, don't draw anything!
	int max_x = std::min(screenWidth - 1, (int)std::max({ pixelTri.v0.position.x,pixelTri.v1.position.x,pixelTri.v2.position.x }));
	int min_x = std::max(0, (int)std::min({ pixelTri.v0.position.x,pixelTri.v1.position.x,pixelTri.v2.position.x }));
	//finding max and min of y
	int max_y = std::min(screenheight - 1, (int)std::max({ pixelTri.v0.position.y,pixelTri.v1.position.y,pixelTri.v2.position.y }));
	int min_y = std::max(0, (int)std::min({ pixelTri.v0.position.y,pixelTri.v1.position.y,pixelTri.v2.position.y }));
	//loop through the entire box(Barycentric method) and check if the pixel is inside the triangle or not using edge function, if its inside then colour that pixel
	for (int i = min_y; i <= max_y; ++i) {
		for (int j = min_x; j <= max_x; ++j) {
			//Edge function
			float w2 = (pixelTri.v1.position.x - pixelTri.v0.position.x) * (i - pixelTri.v0.position.y) - (pixelTri.v1.position.y - pixelTri.v0.position.y) * (j - pixelTri.v0.position.x);
			float w0 = (pixelTri.v2.position.x - pixelTri.v1.position.x) * (i - pixelTri.v1.position.y) - (pixelTri.v2.position.y - pixelTri.v1.position.y) * (j - pixelTri.v1.position.x);
			float w1 = (pixelTri.v0.position.x - pixelTri.v2.position.x) * (i - pixelTri.v2.position.y) - (pixelTri.v0.position.y - pixelTri.v2.position.y) * (j - pixelTri.v2.position.x);
			if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
				//Z buffer check
				float area = w0 + w1 + w2;
				if (area == 0) { continue; }
				//turn weights into percentages
				float alpha = w0 / area;
				float beta = w1 / area;
				float gamma = w2 / area;
				float z = alpha * pixelTri.v0.position.z + beta * pixelTri.v1.position.z + gamma * pixelTri.v2.position.z;
				int pixelIndex = i * screenWidth + j;
				if (z < zBuffer[pixelIndex]) {
					zBuffer[pixelIndex] = z;

					//interpolate the normals
					Vec3 pixelNormal(
						alpha * pixelTri.v0.normal.x + beta * pixelTri.v1.normal.x + gamma * pixelTri.v2.normal.x,
						alpha * pixelTri.v0.normal.y + beta * pixelTri.v1.normal.y + gamma * pixelTri.v2.normal.y,
						alpha * pixelTri.v0.normal.z + beta * pixelTri.v1.normal.z + gamma * pixelTri.v2.normal.z
					);
					pixelNormal = pixelNormal.normalize();
					Vec3 LightDir(0.5f, 1.0f, 0.2f); // Light direction
					LightDir = LightDir.normalize();
					float dotProduct = pixelNormal.dot(LightDir);

					float ambient = 0.15f; // Ambient lighting factor hack
					float sun = std::max(0.0f, dotProduct); // Sunlight factor
					float totalLight = std::min(1.0f, ambient + sun); // Total light factor); 
					//colour the pixel here
					// 1. Calculate the exact U and V for this specific pixel using Barycentric weights!
					float u = alpha * pixelTri.v0.u + beta * pixelTri.v1.u + gamma * pixelTri.v2.u;
					float v = alpha * pixelTri.v0.v + beta * pixelTri.v1.v + gamma * pixelTri.v2.v;

					// 2. Translate the 0.0-1.0 percentage into an exact image pixel coordinate
					int tex_x = (int)(u * (tex.width - 1));
					int tex_y = (int)(v * (tex.height - 1));

					// Safety Catch (forces the numbers to stay inside the image bounds)
					tex_x = std::max(0, std::min(tex_x, tex.width - 1));
					tex_y = std::max(0, std::min(tex_y, tex.height - 1));

					// 3. Steal the color from the image array!
					int texIndex = tex_y * tex.width + tex_x;
					Vec3 textureColor = tex.pixels[texIndex];

					// 4. Apply your Lambert Shadows to the stolen color!
					float final_R = textureColor.x * totalLight;
					float final_G = textureColor.y * totalLight;
					float final_B = textureColor.z * totalLight;

					// 5. Paint the canvas!
					canvas[pixelIndex] = color(final_R, final_G, final_B);

				}

			}
		}
	}
}

// 2. Your NEW Texture Loader (OUTSIDE OF MAIN!)
Texture LoadTexture(const std::string& filename) {
	Texture tex;
	std::ifstream file(filename);

	// FIX: Check 'file', not 'filename'
	if (!file.is_open()) {
		std::cout << "ERROR: Texture File could not Open: " << filename << std::endl;
		return tex;
	}

	std::string magicnumber;
	file >> magicnumber;
	file >> tex.width >> tex.height;

	int maxColor;
	file >> maxColor;

	tex.pixels.resize(tex.width * tex.height);

	// FIX: Added 'int' before 'i'
	for (int i = 0; i < tex.width * tex.height; ++i) {
		int r, g, b;
		file >> r >> g >> b;
		float float_r = r / 255.0f;
		float float_g = g / 255.0f;
		float float_b = b / 255.0f;

		tex.pixels[i] = color(float_r, float_g, float_b);
	}

	file.close();
	return tex;
}

//conversion of AOS to SOA
Mesh_SoA convert_AoS_to_SoA(const std::vector<Triangle>& aos_triangles) {
	Mesh_SoA soa;
	int numVertices = aos_triangles.size() * 3; // Each triangle has 3 vertices
	soa.positions.reserve(numVertices);
	soa.normals.reserve(numVertices);
	soa.u.reserve(numVertices);
	soa.v.reserve(numVertices);
	for (int i = 0; i < aos_triangles.size(); ++i) {
		//rip out v0
		soa.positions.push_back(aos_triangles[i].v0.position);
		soa.normals.push_back(aos_triangles[i].v0.normal);
		soa.u.push_back(aos_triangles[i].v0.u);
		soa.v.push_back(aos_triangles[i].v0.v);
		//rip out v1
		soa.positions.push_back(aos_triangles[i].v1.position);
		soa.normals.push_back(aos_triangles[i].v1.normal);
		soa.u.push_back(aos_triangles[i].v1.u);
		soa.v.push_back(aos_triangles[i].v1.v);
		//rip out v2
		soa.positions.push_back(aos_triangles[i].v2.position);
		soa.normals.push_back(aos_triangles[i].v2.normal);
		soa.u.push_back(aos_triangles[i].v2.u);
		soa.v.push_back(aos_triangles[i].v2.v);
	}
	return soa;
}

int main() {

	int width = 800;
	int height = 600;
	//for z buffer
	std::vector<float> zBuffer(width * height, std::numeric_limits<float>::max());
	// 2. Buy the Canvas (Filled with Pitch Black: 0,0,0)
	std::vector<color> canvas(width * height, color(0.0f, 0.0f, 0.0f));

	Texture myTexture = LoadTexture("C:/Users/jprag/OneDrive/Desktop/Graphics Programming/steve_face_texture.ppm");

	Vec3 eye(3.0f, 3.0f, 3.0f);
	Vec3 target(0.0f, 0.0f, 0.0f);
	Vec3 up(0.0f, 1.0f, 0.0f);
	Mat4 Model;   //identity matrix
	Mat4 view = lookAt(eye, target, up);
	Mat4 projection = Mat4::perspective(75.0f, (float)width / (float)height, 0.01f, 100.0f);
	Mat4 MVP = projection.multiply(view.multiply(Model)); //remember to read from right to left for matrix multiplication

	std::vector<Triangle> screenTriangles;


	std::vector<Triangle> cube_triangles = {
		// FRONT FACE (Facing +Z) -> Normal: (0, 0, 1)
		{
			{ Vec3(-1, -1,  1), Vec3(0, 0, 1), 0.0f, 1.0f }, // Bottom-Left
			{ Vec3(1, -1,  1), Vec3(0, 0, 1), 1.0f, 1.0f }, // Bottom-Right
			{ Vec3(1,  1,  1), Vec3(0, 0, 1), 1.0f, 0.0f }  // Top-Right
		},
		{
			{ Vec3(-1, -1,  1), Vec3(0, 0, 1), 0.0f, 1.0f }, // Bottom-Left
			{ Vec3(1,  1,  1), Vec3(0, 0, 1), 1.0f, 0.0f }, // Top-Right
			{ Vec3(-1,  1,  1), Vec3(0, 0, 1), 0.0f, 0.0f }  // Top-Left
		},

		// BACK FACE (Facing -Z) -> Normal: (0, 0, -1)
		{
			{ Vec3(1, -1, -1), Vec3(0, 0, -1), 0.0f, 1.0f },
			{ Vec3(-1, -1, -1), Vec3(0, 0, -1), 1.0f, 1.0f },
			{ Vec3(-1,  1, -1), Vec3(0, 0, -1), 1.0f, 0.0f }
		},
		{
			{ Vec3(1, -1, -1), Vec3(0, 0, -1), 0.0f, 1.0f },
			{ Vec3(-1,  1, -1), Vec3(0, 0, -1), 1.0f, 0.0f },
			{ Vec3(1,  1, -1), Vec3(0, 0, -1), 0.0f, 0.0f }
		},

		// LEFT FACE (Facing -X) -> Normal: (-1, 0, 0)
		{
			{ Vec3(-1, -1, -1), Vec3(-1, 0, 0), 0.0f, 1.0f },
			{ Vec3(-1, -1,  1), Vec3(-1, 0, 0), 1.0f, 1.0f },
			{ Vec3(-1,  1,  1), Vec3(-1, 0, 0), 1.0f, 0.0f }
		},
		{
			{ Vec3(-1, -1, -1), Vec3(-1, 0, 0), 0.0f, 1.0f },
			{ Vec3(-1,  1,  1), Vec3(-1, 0, 0), 1.0f, 0.0f },
			{ Vec3(-1,  1, -1), Vec3(-1, 0, 0), 0.0f, 0.0f }
		},

		// RIGHT FACE (Facing +X) -> Normal: (1, 0, 0)
		{
			{ Vec3(1, -1,  1), Vec3(1, 0, 0), 0.0f, 1.0f },
			{ Vec3(1, -1, -1), Vec3(1, 0, 0), 1.0f, 1.0f },
			{ Vec3(1,  1, -1), Vec3(1, 0, 0), 1.0f, 0.0f }
		},
		{
			{ Vec3(1, -1,  1), Vec3(1, 0, 0), 0.0f, 1.0f },
			{ Vec3(1,  1, -1), Vec3(1, 0, 0), 1.0f, 0.0f },
			{ Vec3(1,  1,  1), Vec3(1, 0, 0), 0.0f, 0.0f }
		},

		// TOP FACE (Facing +Y) -> Normal: (0, 1, 0)
		{
			{ Vec3(-1,  1,  1), Vec3(0, 1, 0), 0.0f, 1.0f },
			{ Vec3(1,  1,  1), Vec3(0, 1, 0), 1.0f, 1.0f },
			{ Vec3(1,  1, -1), Vec3(0, 1, 0), 1.0f, 0.0f }
		},
		{
			{ Vec3(-1,  1,  1), Vec3(0, 1, 0), 0.0f, 1.0f },
			{ Vec3(1,  1, -1), Vec3(0, 1, 0), 1.0f, 0.0f },
			{ Vec3(-1,  1, -1), Vec3(0, 1, 0), 0.0f, 0.0f }
		},

		// BOTTOM FACE (Facing -Y) -> Normal: (0, -1, 0)
		{
			{ Vec3(-1, -1, -1), Vec3(0, -1, 0), 0.0f, 1.0f },
			{ Vec3(1, -1, -1), Vec3(0, -1, 0), 1.0f, 1.0f },
			{ Vec3(1, -1,  1), Vec3(0, -1, 0), 1.0f, 0.0f }
		},
		{
			{ Vec3(-1, -1, -1), Vec3(0, -1, 0), 0.0f, 1.0f },
			{ Vec3(1, -1,  1), Vec3(0, -1, 0), 1.0f, 0.0f },
			{ Vec3(-1, -1,  1), Vec3(0, -1, 0), 0.0f, 0.0f }
		}
	};

	Mesh_SoA fastMesh = convert_AoS_to_SoA(cube_triangles);

	// --- START STOPWATCH ---
	auto start_time = std::chrono::high_resolution_clock::now();


	for (int i = 0; i < cube_triangles.size(); ++i) {
		std::vector<Vertex> projected_Triangle;
		Vertex p0, p1, p2;
		int vIndex = i * 3;
		p0.position = MVP.mult_4dmat_vec3(fastMesh.positions[vIndex]);
	    p0.normal= fastMesh.normals[vIndex];
		p0.u = fastMesh.u[vIndex]; 
		p0.v = fastMesh.v[vIndex];
		projected_Triangle.push_back(p0);   // MVP
		p1.position = MVP.mult_4dmat_vec3(fastMesh.positions[vIndex + 1]);
	    p1.normal= fastMesh.normals[vIndex + 1];
		p1.u = fastMesh.u[vIndex + 1];
		p1.v = fastMesh.v[vIndex + 1];
		projected_Triangle.push_back(p1);
		p2.position = MVP.mult_4dmat_vec3(fastMesh.positions[vIndex + 2]);
	    p2.normal= fastMesh.normals[vIndex + 2];
		p2.u = fastMesh.u[vIndex + 2];
		p2.v = fastMesh.v[vIndex + 2];
		projected_Triangle.push_back(p2);
		std::vector<Vertex> clippedPolygon = ClipPolygon(projected_Triangle);    //clipping
		std::vector<Triangle> finalTriangles = Triangulate(clippedPolygon);    //triangulation

		if (finalTriangles.empty()) {
			continue;
		}
		for (int i = 0; i < finalTriangles.size(); ++i) {
			Triangle t = finalTriangles[i];

			Triangle pixelTriangle;
			pixelTriangle.v0.position.x = (t.v0.position.x + 1.0) * 0.5 * width;
			pixelTriangle.v0.position.y = (1.0 - (t.v0.position.y)) * 0.5 * height; 
			pixelTriangle.v0.position.z = t.v0.position.z;
			pixelTriangle.v0.normal = t.v0.normal;
			pixelTriangle.v0.u = t.v0.u; // <--- ADD THIS
			pixelTriangle.v0.v = t.v0.v; // <--- ADD THIS

			pixelTriangle.v1.position.x = (t.v1.position.x + 1.0) * 0.5 * width;
			pixelTriangle.v1.position.y = (1.0 - (t.v1.position.y)) * 0.5 * height;
			pixelTriangle.v1.position.z = t.v1.position.z;
			pixelTriangle.v1.normal = t.v1.normal;
			pixelTriangle.v1.u = t.v1.u; // <--- ADD THIS
			pixelTriangle.v1.v = t.v1.v; // <--- ADD THIS

			pixelTriangle.v2.position.x = (t.v2.position.x + 1.0) * 0.5 * width;
			pixelTriangle.v2.position.y = (1.0 - (t.v2.position.y)) * 0.5 * height;
			pixelTriangle.v2.position.z = t.v2.position.z;
			pixelTriangle.v2.normal = t.v2.normal;
			pixelTriangle.v2.u = t.v2.u; // <--- ADD THIS
			pixelTriangle.v2.v = t.v2.v; // <--- ADD THIS
			Draw_triangle(pixelTriangle, width, height, zBuffer, canvas, myTexture);

			screenTriangles.push_back(pixelTriangle);
		}
	}
	// --- STOP STOPWATCH ---
	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> render_time = end_time - start_time;
	std::cout << "SoA Render Time: " << render_time.count() << " ms\n";

	// ...
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
	for (int i = 0; i < width * height; ++i) {
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









