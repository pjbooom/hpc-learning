#pragma once
#include<ostream>
#include<cmath>//need it for tan function in perspective matrix
#include "math/vec.h" // need this to define a function that multiplies 4D matrix with 3D vector
struct Vec4 {    //A simple container for our 4D coordinates
    float x;
    float y;
    float z;
    float w;
};
struct Mat4 {
	//The OpenGL function that takes your matrix is called glUniformMatrix4fv. It explicitly demands a flat pointer to a 1D array (const float*). If you build your engine using m[4][4], you have to constantly perform ugly pointer casting (like &m[0][0]) every single time you talk to the GPU, which can lead to memory alignment bugs. m[16] plugs directly into OpenGL with zero translation needed
	float m[16];
	//constructor
	Mat4() {
		for (int i = 0; i < 16; ++i) {
			m[i] = 0.0f;
		}
		//m[0], m[5], m[10], m[15] = 1.0f; Your brain is thinking in Python, where you can assign multiple variables at once.In C++, this uses the comma operator, which is a silent killer.The C++ compiler reads that line like this:"Evaluate m[0]. Okay, done. Throw it away.""Evaluate m[5]. Okay, done. Throw it away.""Evaluate m[10]. Okay, done. Throw it away.""Set m[15] to 1.0f."
		m[0] = 1.0f;
		m[5] = 1.0f;
		m[10] = 1.0f;
		m[15] = 1.0f;
	}
	//creating multiply function between 2 matrices
	//see from the constructor above you have a Identity matrix clearly, now how do we do this fucking function
	//well ok imagine 4*4 identity matirx. now since CPU stores this as array of 16 numbers see, it goes from col to col like first element in that array is 1st element in identity matirx
	//now, the second element is actually directly down of that first element like dont go to right, go to down, like column wise
	//so, In a Column-Major flat array, every column is a chunk of 4 numbers.To find the starting index of any column, you multiply by 4. (Column 3 starts at 3 * 4 = 12).To drop down to a specific row within that column, you just add the row number.Master Formula : Index = (Column * 4) + Row
	//multiplication matrix with matrix
    Mat4 multiply(const Mat4& other) const {
		//creating another mat4 to store result
		Mat4 result;
		for (int col = 0; col < 4; ++col) {
			for (int row = 0; row < 4; ++row){
				float sum = 0.0f;
				for (int i = 0; i < 4; ++i) {
					sum += m[i * 4 + row]*(other.m[col * 4 + i]);
				}
				result.m[col * 4 + row] = sum;
			}
		}
		return result;
	}
	//overloading function for multiplying matrix with vector, see here we are multiplying 4*4 matrix with 4D vector, so we will get 4D vector as result, so we return Vec4
    Vec4 multiply(Vec4 v) const {
        Vec4 res;

        // Row 1: Calculates New X (m0, m4, m8, m12)
        res.x = (m[0] * v.x) + (m[4] * v.y) + (m[8] * v.z) + (m[12] * v.w);

        // Row 2: Calculates New Y (m1, m5, m9, m13)
        res.y = (m[1] * v.x) + (m[5] * v.y) + (m[9] * v.z) + (m[13] * v.w);

        // Row 3: Calculates New Z (m2, m6, m10, m14)
        res.z = (m[2] * v.x) + (m[6] * v.y) + (m[10] * v.z) + (m[14] * v.w);

        // Row 4: THE W-TRAP! Calculates New W (m3, m7, m11, m15)
        res.w = (m[3] * v.x) + (m[7] * v.y) + (m[11] * v.z) + (m[15] * v.w);

        return res;
    }
	// 4. The Inverse (The Camera/Undo Button)    CARMERS RULE (NO NEED TO MEMORISE THIS, JUST COPY AND PASTE IT INTO YOUR ENGINE, AND THEN CALL IT WHEN YOU NEED TO INVERT A MATRIX)
    Mat4 inverse() const {
        Mat4 inv;

        // Step 1: Calculate the massive cross-multiplication pairs (Adjugate)   like caluclating det(3*3) matrix first
        inv.m[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];  //remeber here we didnt multiply the starting element here, visulaise how we find detereminant of 3*3 like me multiply the elemnt(det 2*2) right here we didnt do multiply element. so we do it i.e., m[0]*inv.m[0] in step 2
        inv.m[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
        inv.m[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
        inv.m[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

        inv.m[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
        inv.m[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
        inv.m[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
        inv.m[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

        inv.m[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
        inv.m[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
        inv.m[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
        inv.m[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

        inv.m[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
        inv.m[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
        inv.m[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
        inv.m[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

        // Step 2: Calculate the Determinant (The Volume)
        // We do this by multiplying the first row of the original matrix by the first column of the inverse we just built
        float det = m[0] * inv.m[0] + m[1] * inv.m[4] + m[2] * inv.m[8] + m[3] * inv.m[12];

        // Step 3: The Safety Check
        // If the volume is exactly 0, space is collapsed. We can't invert it.
        // We just return the blank Identity Matrix to prevent a crash.
        if (det == 0.0f) {
            return Mat4();
        }

        // Step 4: Finalize the Inverse
        // Divide every single number in the new matrix by the Determinant
        det = 1.0f / det; // (Multiplication is faster for the CPU than 16 division operations) Mathematically, multiplying by $0.5$ is the exact same thing as dividing by $2$, but for the computer hardware, it is almost ten times faster!

        for (int i = 0; i < 16; i++) {
            inv.m[i] = inv.m[i] * det;
        }

        return inv;
    }
    //oh its simple, the inverse we calucalted is along rows, so we transpose them to get them along columns so when we multiply matrix with its inverse we get identity matrix, thast it, its so simple
    // 5. The Transpose (The Mirror Flip)
    Mat4 transpose() const {
        Mat4 res;

        // Col 0 becomes Row 0
        res.m[0] = m[0]; res.m[1] = m[4]; res.m[2] = m[8];  res.m[3] = m[12];
        // Col 1 becomes Row 1
        res.m[4] = m[1]; res.m[5] = m[5]; res.m[6] = m[9];  res.m[7] = m[13];
        // Col 2 becomes Row 2
        res.m[8] = m[2]; res.m[9] = m[6]; res.m[10] = m[10]; res.m[11] = m[14];
        // Col 3 becomes Row 3
        res.m[12] = m[3]; res.m[13] = m[7]; res.m[14] = m[11]; res.m[15] = m[15];

        return res;
    }
    //building perspective matrix where it handles FOV, perspective dividing,etc like this 4*4 matrix 
    //The job of the perspective() function is purely translation and scaling. When you passed in NearPlane = 0.1f and FarPlane = 100.0f, you told the matrix: "Hey, take anything between 0.1 and 100.0 in the real world, and squish it down mathematically so it fits perfectly between -1.0 and 1.0 for the GPU."
    //so here nearplane and farplane is like camera sees from 0.1f oto 100.0f, like minecrast steve can see mountains till 100.0f but GPU says 'i dont want your meesy 0.1 to 100 convert those to -1 to +1 for me, so -1=0.1 and 1=100 so anything inbetween needs to get converted to the GPU(-1 to +1) co ordinates.
    static Mat4 perspective(float FovDegrees, float AspectRatio, float NearPlane, float FarPlane) {
        Mat4 res;
        //here res is now an identity matrix, tahnk to Mat4 fucntion which we wrote. now lets clean this to 0.0f in all slots 9this is mainly for perspetive diving, we havent done FOV yet)
        for (int i = 0; i < 16; ++i) {
            res.m[i] = 0.0f;
        }
        //now first let us do 4*4 perspective matrix when multiplied with mario's corodinates [x,y,z,w] wight we want [x,y,z,z] so taht we can divide by z to get perspetcive diving
        // actually we want w=-z like imagine mario moving in screen hes going -z direction right so. now see 4*4 perspective matrix multiplies with [x,y,z,w] now, new w=-z
        //see here last row of 4*4 persepctive matrix get multplied wiht [x,y,z,w] to get elemnt in w slot of mario co ordinates (normal matrix mult)
		res.m[11] = -1.0f; //this is the only element in last row of perspective matrix which is -1.0f rest all are 0.0f, so when we do matrix multiplication with mario's co ordinates [x,y,z,w] we get w=-z
        
        //FOV and aspect ratio stuff
		//to form FOV we use tan, see tan is opposite/adjacent, so if we have FOV of 90 degrees, then tan(90/2) = 1.0f, so we want to scale x and y by 1.0f, so that when mario moves in z direction his x and y also scale up by 1.0f, so that we get perspective diving also it should be in degress so humans can change FOV easily in degrees, but internally tan accpets only radians, so we have to convert form degrees to radians
		float tanHalfFOV = std::tan((FovDegrees / 2.0f) * (3.14159265f / 180.0f)); //converting degrees to radians and also diving FOV by 2 cause Trigonometry (like sine, cosine, and tangent) only works on Right Triangles (triangles with a perfect $90^\circ$ corner).Your camera's vision isn't a right triangle. It is a giant "V" shape spreading out from the player's eyes.To use tan(), we have to draw a straight line right down the middle of the player's screen, slicing that "V" perfectly in half. That slice creates a Right Triangle. Because we sliced the vision in half to make the math work, we have to cut the FOV angle in half too.
        //The Y-Axis (Height): We put the result into Slot 5. The X - Axis(Width) : We put the result into Slot 0, but we also divide it by the aspectRatio so things don't look stretched on your widescreen monitor.
        res.m[5] = 1.0f / tanHalfFOV; //Finding the New Y (Height):The matrix uses Row 2 to find height. Row 2 is m[1], m[5], m[9], and m[13].The math is: ({Slot 1} * X) + ({Slot 5} * Y) + ({Slot 9} * Z) + ({Slot 13} * W) = New Y .If we want to squish Mario's Y-height, we have to put our FOV math exactly into Slot 5 so it multiplies directly against his original Y.
		res.m[0] = 1.0f / (AspectRatio * tanHalfFOV);    //same reason but here its Width squishing and above one was height squishing , so we put it in slot 0 which is Row 1, and also we divide it by aspect ratio to prevent stretching on widescreen monitors
		//only remaining thing is Z which we will us eit to generate farPLane and NearPlane like when to stop rendering like in minecraft you can see far away mountains but not infinitely far away, so we use far plane to stop rendering after certain distance, and near plane to prevent rendering of things which are too close to camera like in minecraft when you go inside a block it just disappers because its too close to camera, so we use near plane for that
        //2 steps for that- We need to grab Mario's Z-coordinate, and mathematically squash it down so that the entire game world (from the camera's nearPlane all the way to the distant farPlane) fits perfectly inside the GPU's tiny [-1, 1] cube(its like renderign box, like render anything which fits this cube like steve from minecraft what we can see is GPU cube set.To squash a massive line down into a tiny box, we need to do two things to it:1)Scale it: Multiply it by a fraction to shrink it.2)Offset it: Add or subtract a number to slide it backwards so it centers perfectly inside the GPU's box.
        //for scale- we use slot 10, which multiplies with original mario's Z corodinate and for offset we use slot 14 which multiplies with mario's original w coordinates, which is 1.0 right, so we can use that and add it finally to bring back our result into GPU cube.
        res.m[10] = -(FarPlane + NearPlane) / (FarPlane - NearPlane);
        res.m[14] = -(2.0f * FarPlane * NearPlane) / (FarPlane - NearPlane);

        return res;
    }
    inline Vec3 mult_4dmat_vec3(const Vec3& v) const {
		float x = v.x * m[0] + v.y * m[4] + v.z * m[8] + 1.0f* m[12];
		float y = v.x * m[1] + v.y * m[5] + v.z * m[9] + 1.0f * m[13];
		float z = v.x * m[2] + v.y * m[6] + v.z * m[10] + 1.0f * m[14];
		float w = v.x * m[3] + v.y * m[7] + v.z * m[11] + 1.0f * m[15];
        //perspective divide
        if (w!=0.0f){
            x /= w;
            y /= w;
            z /= w;
		}
        return Vec3(x, y, z);
    }
};
