**Objective**: Implement robust 4x4 matrix utilities, compose transforms and ensure numerical stability.

**Micro-tasks (execute in order):**
1. Implement `include/math/mat4.h` with Mat4, multiply, inverse, transpose.
2. Write `examples/transform_cube.cpp`: model-view-projection manually; output 2D coords.
3. Unit tests `tests/test_mat4.cpp`: identity invariants, inversion checks.
4. Experiment near-singular matrices; document in `docs/matrix_stability.md`.
5. Write mapping note for GPU matrix representation.
   
**Deliverable**: include/math/mat4.h, examples/transform_cube.cpp, tests/test_mat4.cpp, docs/matrix_stability.md

**Mastery checks:**
1. Matrix tests pass (inversion tolerance).
2. MVP pipeline projects cube correctly.
3. Document homogeneous division and clip-space mapping.