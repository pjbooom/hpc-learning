//Imagine if your physics code and your graphics code both try to read the vec.h dictionary at the same time. The compiler would panic and say, "Wait, you're defining Vec3 twice!" #pragma once is a strict rule that tells the compiler: "No matter how many times other files ask for this dictionary, only read it ONCE."
#pragma once
//we need cmath for the sqrt function in length() function
#include <cmath>
//we need iostream for the operator<< function to print vector in main.cpp
#include <ostream>
// Here struct means You are telling the CPU: "Whenever I say Vec3, I want you to carve out exactly 12 bytes of memory—4 bytes for an X float, 4 bytes for a Y float, and 4 bytes for a Z float."
struct Vec3 {
	union {    // The anonymous union forces everything inside it to share the same memory space
		struct { float x, y, z; };  // Name Tag Group 1: Coordinates
		struct { float r, g, b; };  // Name Tag Group 2: Colors
		//Because r and x are literally sharing the same electrons in your computer's RAM, you don't have to change your math functions at all. Your below function still does math using x, y, z
	};

	// Constructor, which makes initally x,y,z as 0. Cause when you defined x,y,z above it might contain grabage value before. so this makes sure it makes it 0.
	Vec3(float x_input = 0.0f, float y_input = 0.0f, float z_input = 0.0f) {
		x = x_input;
		y = y_input;
		z = z_input;
	}

	//defining + operator for Vec3, cause in main.cpp when we say add 2 vectors(it doesnt know how to), it only knows simple 5+4, but not vectors. so we define addition operators specifically for vectors
	//The & symbol means "pass-by-reference." Instead of making a slow, heavy copy of b in the RAM, it just looks directly at b's original memory address. This makes your game engine lightning fast.
	//The const keyword is a promise: "I will only look at b, I promise not to accidentally change its values." same for the other const after brackets "promise i will not chnage anything from vector 'a' also
    //return Vec3(...): Here, you are calling the Constructor you wrote in the last step! You are feeding it the x+x, y+y, and z+z math, and it constructs the final answer.

	Vec3 operator+(const Vec3& other) const {
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	//defining - operator for Vec3
	
	Vec3 operator-(const Vec3& other) const {
		return Vec3(x - other.x, y - other.y, z - other.z);
	}

	//defining lenght(magnitude) of vector

	float length() const {
		//it's slightly safer to write std::sqrt instead of just sqrt, because it guarantees the compiler uses the highly optimized standard library version from <cmath>
		return std::sqrt((x * x) + (y * y) + (z * z));
	}

	//defining NORMALISATION(unit) of vector 

	Vec3 normalize() const {
		// First, we find the length of the vector using the length() function we just defined.
		float l = length();
		// we need to also check if lenght is 0, then we cant divide by 0, so we return a zero vector in that case, otherwise we divide vector by its length to get normalised vector(unit vector)
		//Catching that if (l == 0) edge case is massive. In a 3D game, if a player is standing perfectly still, their velocity vector is [0, 0, 0]. If your physics engine tries to normalize their velocity to find out which way they are facing, and you divide by zero, the CPU generates a NaN (Not a Number). That NaN infects every other calculation, and suddenly your player's coordinates become [NaN, NaN, NaN] and they completely disappear from the game world. You just prevented your first engine crash.
		if (l == 0) {
			return Vec3(0.0f, 0.0f, 0.0f);
		}
		else {
			return Vec3(x / l, y / l, z / l);
		}
	}

	//defining dot product of vector(scalar)

	float dot(const Vec3& other) const {
		return ((x * other.x) + (y * other.y) + (z * other.z));
	}

	//defining cross product of vector(vector)

	Vec3 cross(const Vec3& other) const {
		return Vec3((y * other.z) - (z * other.y), (z * other.x) - (x * other.z), (x * other.y) - (y * other.x));
	}

	//If you go into main.cpp right now and type std::cout << myVector;, the compiler will panic. It knows how to print text, and it knows how to print numbers. But when it sees your custom 12-byte Vec3 Lego brick, it has no idea what to do. Should it print the memory address? Should it print X, Y, and Z?
	//We have to explicitly teach the << symbol how to print a Vec3. So we need to take control of '<<' using 'operator<<'

	//The technical term for this conveyor belt is std::ostream (Standard Output Stream). When you type std::cout << "Hello", you are taking the word "Hello" and throwing it onto the cout conveyor belt.
	//inline(pacifier)-if many files read this line, then dont panic and say I defined that many times, just copy and paste silently.
	
	//why do we write std::ostream& at the start? after inline
	//Because of "chaining." Imagine you write this in your game:
	//std::cout << myVector << " is my velocity!" << std::endl;

	//The computer reads left to right.First, it processes std::cout << myVector.If your function just printed the vector and stopped dead, the rest of the line(<< " is my velocity!") would crash because it has no stream left to connect to.
	//By returning the stream itself, you pass the wire down the line so the next << can keep pushing data to the screen
	//also ostream& os or Vec3& v means as you know '&' means pass by refrence so passing the stream directly from memory and not making copies in RAM and 'os' or 'v' is like calling ostream as os and Vec3 as v
	//Why it's asking for a friend
	//When you wrote the operator+ function, you put it inside the Vec3 struct.That works perfectly because when you type a + b, the vector a is on the left side.The Vec3 struct says, "Cool, I am on the left, so I own this math operation."

     //But look at how we print things :
	//std::cout << myVector;

	//What is on the left side of the << symbol ? std::cout.
	//Because std::cout(the text stream) is on the left, the operation technically belongs to the stream, not your vector(here its 'a' cause a+b means its 'a''s house BABYY).

		//If you put the operator<< function inside your Vec3 struct, C++ panics and says, "Wait, a text stream is trying to start an operation inside a vector's house. That's illegal!"
	//In C++, if an outside function (like printing to a stream) needs to reach inside your struct to read its x, y, and z variables, you can grant it VIP access by calling it a friend.
   //It tells the struct : "This function doesn't belong to you, but treat it like a friend and let it read your data."
	//NOTE- actually By making it a friend inside the struct, you don't need the inline keyword anymore, and the compiler knows exactly how to handle the permissions.

	friend inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
		os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
		return os;
	}

	//lerp formula
	static Vec3 lerp(const Vec3& A, const Vec3& B, float t) {
		return Vec3(A.x + t * (B.x - A.x), A.y + t * (B.y - A.y), A.z + t * (B.z - A.z));
	}
};
