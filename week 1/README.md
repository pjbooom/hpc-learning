**Objective**: Build a robust Vec2/Vec3 C++ math module and microbench the baseline memory effects (AoS vs SoA).
Understand how access patterns affect cache and later GPU coalescing.
**Micro-tasks (execute in order)**:
1. Create new git repo: `mext_gpu_project`. Init `CMakeLists.txt`, `src/`, `include/`, `tests/`.
2. Implement `include/math/vec.h` with Vec3 and inline functions: dot, cross, length, normalize. Add operator
overloads and ostream printing.
3. Write unit tests in `tests/test_vec.cpp`: verify dot orthogonality, normalize tolerance, cross product correctness.
4. Implement `bench/aos_vs_soa.cpp`: arrays of 1e7 Vec3s (AoS) vs SoA; time dot-sum loops; compile with -O3-march=native.
5. Run bench 5 times, record median to `bench/results/vec_layout.csv`.
6. Document `docs/vec_memory.md` describing alignment, cache, and SoA advantages for GPUs.
7. Optional: implement SIMD dot product (intrinsics) or compiler hints.
Deliverable: math/vec.h, tests/test_vec.cpp, bench/aos_vs_soa.cpp, bench/results/vec_layout.csv,
docs/vec_memory.md
Hours/day recommendation: GPU/C++ 3.0h, Physics 1.0h, MinorProj 0.5h, DSA 0.5h, College 1.5h
**Mastery checks**:
1. Unit tests pass.
2. Bench shows measurable AoS vs SoA difference (>10%).
3. Explain why SoA helps GPU coalescing in docs.
