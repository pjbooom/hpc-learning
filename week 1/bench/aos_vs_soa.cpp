#include<iostream>
#include "math/vec.h"

// In this benchmark, we will compare the performance of an Array of Structures (AoS) and a Structure of Arrays (SoA) for storing and manipulating a large number of Vec3 objects. We will measure the time taken to perform a series of operations on both data structures.
// now, we need this below header where <vector> means its just continuolsy explanding list of datas, like infintely large column of datas

#include<vector>

// Now <chrono> is a header C++ time keeeping library, since we are racing 2 large data structures, we need a precise stopwatch to measure how mnay milliseconds it takes for the logic to run

#include<chrono>

// Now, our 'strct Vec3 {' in math/vec.h is an Array of Structures (AoS) because we have a struct Vec3 that contains three float members (x, y, z). So, now its just essentially GROUPED BY VECTORS and its in one single row.
// if we will use diff struct to generate Structure of Arrays (SoA) then we will have 3 separte arrays of one X axix, other Y axis and other Z axis. SO essentially just 3 rows and many columns, GROPUED BY AXIS
//The Logic: You are creating a master block that contains three distinct, massive data streams. One continuous stream for every X value, one for every Y value, and one for every Z value.

struct Vec3_SoA {
	std::vector<float> x;
	std::vector<float> y;
	std::vector<float> z;

	// We need to add constructor to tell the size of the vectors, cause rigth now they are emmpty and we need to tell "I NEED 10 million of these vectors"
    //size_t = This is a special C++ data type used purely for counting things or measuring size. It stands for "Size Type". It is essentially an integer that physically cannot be negative (because you can't have a list with negative 5 items).
	// resize(size,0.0f)= built in function of std::vector that tells the data stream "instantly expand to whatever "size" i GIVE YOU and fill every slot with 0.0f for now

	Vec3_SoA(size_t size) {
		x.resize(size, 0.0f);
		y.resize(size, 0.0f);
		z.resize(size, 0.0f);
	}
};

int main() {

	//Defining variable N to represtn number of vectors we want, here its 10 million

	int N = 10000000;

	//Now we create AoS version of 10 million vectors first, where we take ith vector and get x or y or z value of that particular vector
	//You are telling C++ to create a continuously expanding list (std::vector). The type of data going into this list is your custom Vec3 block. You are naming this list aos_data. By putting (N) at the end, you are commanding the list to immediately carve out exactly 10 million empty slots for 10 million Vec3 blocks
	
	std::vector<Vec3> aos_data(N);

	//You are calling the custom block you just wrote above. You are naming it soa_data and passing N into its constructor. If you look at the constructor you wrote earlier, you know exactly what happens next: it takes that 10000000 and uses it to resize its internal X, Y, and Z streams perfectly.

	Vec3_SoA soa_data(N);

	//Now we will fill some values to those 10 MILLION vectors, cause uptill now they had contained just 0.
	//By writing size_t i = 0, you are saying: "Create a counter named i that is strictly a positive integer, start it at 0, and as soon as this loop finishes, destroy i so it doesn't clutter up the system."
	//When you are only counting to 10, it doesn't matter at all. But you are running this loop 10 million times. If you use i++, the system technically has to generate and destroy 10 million useless temporary copies of that number. By using ++i, you are sending a much cleaner, more direct signal to just increase the value and move on.

	for (size_t i = 0; i < N; ++i) {

		//i is an integer (like 1, 2, 3). But your vectors are built out of float memory (decimals). If you try to shove an integer into a float's slot, the compiler sometimes throws a warning. Putting (float) right in front of i is called casting

		float k = (float)i;

		aos_data[i].x = k;
		aos_data[i].y = k + 1.0f;
		aos_data[i].z = k + 2.0f;

		soa_data.x[i] = k;
		soa_data.y[i] = k + 1.0f;
		soa_data.z[i] = k + 2.0f;
	}

		//calculating Time required for dot product of 10 million vectors for both Aos and Soa
		// Accumulator varaibales to hold total dot product result for both Aos and Soa. Actually compiler will return 0.0ms if we dont use the calculated value in the program. C++ is so smart that it ignores the math entirely and reports 0.0ms, thats why we need total varaible here forcing it to do the math and uisng those variables. SO fucking smart.

		float total_aos_dot = 0.0f;
		float total_soa_dot = 0.0f;

		//starting the timer for AoS dot product calculation, we are using high_resolution_clock which is the most precise clock available in C++. We are storing the start time in a variable called start_aos.

		auto start_aos = std::chrono::high_resolution_clock::now();

		//starting the for loop for calculating dot product "with itself" of 10 million vectors for Aos

		for (size_t i = 0; i < N; ++i) {
			total_aos_dot += aos_data[i].dot(aos_data[i]);
		}

		//stopping the timer for AoS dot product calculation, we are storing the end time in a variable called end_aos. total time is time_aos

		auto end_aos = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> time_aos = end_aos - start_aos;
		//------------------------------------------------------------------------------------------------------------------------------

		//starting the timer for SoA dot product calculation, we are using high_resolution_clock which is the most precise clock available in C++. We are storing the start time in a variable called start_soa.

		auto start_soa = std::chrono::high_resolution_clock::now();

		//starting the for loop for calculating dot product "with itself" of 10 million vectors for Soa

		for (size_t i = 0; i < N; ++i) {
            total_soa_dot += (soa_data.x[i] * soa_data.x[i]) + (soa_data.y[i] * soa_data.y[i]) + (soa_data.z[i] * soa_data.z[i]);
	}

		//stopping the timer for SoA dot product calculation, we are storing the end time in a variable called end_soa. total time is time_soa

		auto end_soa = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> time_soa = end_soa - start_soa;

		//printing the time taken for both SoA and AoS dot product calculations and we are using count() function to get the actual numerical value of the time duration in milliseconds.

		std::cout << "AoS Dot product time:" << time_aos.count() << "ms" << std::endl;
		std::cout << "SoA Dot product time:" << time_soa.count() << "ms" << std::endl;

		//Remember the optimization trick? If you don't print these totals, the compiler realizes you never looked at the answers and will just delete your loops to save time. Printing them forces the CPU to do the work).

		std::cout << "Total AoS Dot product value:" << total_aos_dot << std::endl;
		std::cout << "Total SoA Dot product value:" << total_soa_dot << std::endl;

}
