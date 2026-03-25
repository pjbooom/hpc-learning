# Matrix Stability & Singular Matrices
**Date:** March 2026

## The Experiment
To test the mathematical stability of our custom `Mat4::inverse()` function, I intentionally fed it a "Singular Matrix" (a flattened matrix with a determinant of exactly 0) by setting the Z-scale (`m[10]`) to `0.0f`.

## The Result
When multiplying the singular matrix by its attempted inverse, the matrix failed to return to an Identity Matrix. The engine threw a divide-by-zero error, resulting in corrupted output data:
- `[FAIL] Inverse test failed for element 0. Expected 1, got 4`
- `[FAIL] Inverse test failed for element 5. Expected 1, got 0`
- `[FAIL] Inverse test failed for element 7. Expected 0, got 22`
- `[FAIL] Inverse test failed for element 15. Expected 1, got 21`

## Architecture Takeaway
Because the matrix inverse formula requires dividing by the determinant, a singular matrix causes a divide-by-zero error. In a live 3D rendering pass, this math collapse would cause vertex explosions or a hard crash. Future implementations will check if the determinant is `< epsilon` to safely abort the transformation.
