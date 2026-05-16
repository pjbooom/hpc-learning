# Memory Layout Benchmark: AoS vs. SoA

## ⏱️ Benchmark Results
* **AoS (Array of Structures) Render Time:** 3.5083 ms
* **SoA (Structure of Arrays) Render Time:** 2.1685 ms
* **Performance Gain:** ~38% reduction in render time per frame.

## 🧠 Architectural Analysis: Why is SoA Faster?
The significant performance increase is directly tied to hardware-level memory access and CPU cache utilization. 

When a CPU fetches data from main RAM, it does not pull single variables; it pulls a surrounding block of memory known as a 64-byte **Cache Line**. 

In our original **AoS layout** (`[Position, Normal, U, V]`), data is interleaved. During pipeline stages that only require spatial geometry (such as Matrix Multiplication and View Frustum Clipping), the CPU is forced to load unused Normal and UV data into the ultra-fast L1 cache alongside the Positions. This wastes memory bandwidth and guarantees a high rate of cache misses.

By refactoring the geometry into an **SoA layout** (separate, contiguous arrays for `[Positions]`, `[Normals]`, and `[UVs]`), we achieve perfect **spatial locality**. When the clipping function requests vertex positions, the cache line fills with 100% useful, sequential position data, eliminating bottlenecking.

## 🎮 GPU Buffer Upload Implications
This memory refactor is a critical architectural prerequisite for hardware acceleration. Modern GPUs are massively parallel processors that choke on fragmented, interleaved structs. 

When transitioning this engine to a dedicated Graphics API (OpenGL/Vulkan), the GPU's Input Assembler expects contiguous blocks of homogeneous data in the VRAM. By organizing our system into SoA on the CPU side, we have prepared our geometry to be uploaded as distinct Vertex Buffer Objects (VBOs), ensuring maximum throughput to the GPU cores.