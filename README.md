# ⚙️ Custom C++ Software Rasterizer

A high-performance, from-scratch 3D rendering engine built entirely on the CPU using pure C++. 

This project was developed as a deep-dive into low-level computer graphics, linear algebra, and hardware memory architecture. **No external rendering APIs (like OpenGL, DirectX, or Vulkan) were used for the geometry pipeline.** Every matrix multiplication, clipping calculation, and pixel interpolation was engineered from the ground up.

## 🚀 Core Features & Pipeline

### 1. Custom Math & Linear Algebra (`math/`)
* **Vectors & Matrices:** Custom `Vec3`, `Vec4`, and `Mat4` classes.
* **The MVP Pipeline:** Full implementation of Model, View (`lookAt`), and Perspective Projection matrices to translate Local Space geometry into Normalized Device Coordinates (NDC).

### 2. The Geometry Pipeline
* **Sutherland-Hodgman Clipping:** Dynamically clips geometry against the near-plane to prevent memory access violations and vertex wrapping.
* **Bresenham's Line Algorithm:** Highly optimized, integer-only algorithm for perspective-correct wireframe rendering.
* **Barycentric Rasterization:** Custom pixel-filling algorithm using barycentric weights to interpolate data accurately across triangle faces.

### 3. Illumination & Texturing
* **Custom Image Parser:** Built a raw text parser to read and allocate ASCII `.ppm` image files into RAM without external image libraries.
* **Texture Mapping:** Maps 2D texture coordinates (UVs) onto 3D projected planes using perspective-correct barycentric interpolation.
* **Lambert Directional Shading:** Real-time dot-product lighting calculations based on interpolated vertex normals.
* **Z-Buffering:** Custom depth-buffer implementation to track spatial occlusion.

### 4. Memory Architecture & Optimization
* **AoS to SoA Refactoring:** Transitioned the geometry storage pipeline from an Array of Structures (AoS) to a Structure of Arrays (SoA).
* **Performance:** By prioritizing spatial locality and optimizing CPU Cache-Line reads during the spatial transformation phases, the engine achieved a **38% reduction in render time per frame** (from 3.5ms down to 2.1ms).

## 📂 Current Roadmap
This engine was built following a custom, rigorous 6-week architecture syllabus. 

- [x] Week 1: Core Linear Algebra & Image Output
- [x] Week 2: MVP Matrices & Wireframe Rendering
- [x] Week 3: Solid Rasterization & Z-Buffering
- [x] Week 4: Transformation Hierarchies
- [x] Week 5: Texturing, Lighting, & Memory Layout (SoA)
- [ ] **Week 6 (In Progress):** `.obj` File Parsing & Backface Culling

## 🛠️ Build Instructions
Because this engine relies on zero external graphics libraries, compiling is incredibly lightweight.
1. Requires a standard C++17 compiler (MSVC, GCC, or Clang).
2. Compile all `.cpp` files in the `src/` directory.
3. The engine outputs rendered frames as raw `.ppm` files to the local directory.
