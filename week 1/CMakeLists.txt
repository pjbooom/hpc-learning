# Tell CMake the minimum version required
cmake_minimum_required(VERSION 3.10)

# Name your Engine
project(MEXT_Engine)

# Tell the compiler where your math headers will live
include_directories(include)

# ---------------------------------------------------------
# INSTRUCTION 1: Build the Main Engine
# Only use main.cpp because it has its own int main()
add_executable(MEXT_Engine src/main.cpp)

# ---------------------------------------------------------
# INSTRUCTION 2: Build the Math Tester
# Only use test_vec.cpp because it has its own int main()
add_executable(test_vec tests/test_vec.cpp)

# ---------------------------------------------------------
# INSTRUCTION 3: Build the Data Structure Benchmark
# Only use aos_vs_soa.cpp because it has its own int main()
add_executable(bench_vec bench/aos_vs_soa.cpp)
