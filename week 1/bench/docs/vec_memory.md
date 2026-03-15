# Vector Math & Memory Layout Benchmark

## The Objective
To profile the performance delta between Array of Structures (AoS) and Structure of Arrays (SoA) memory layouts when computing dot products across 10 million 3D vectors. 

## The Results (Release Build, -O3 Optimization)
* **AoS Median Time:** 15.2555 ms
* **SoA Median Time:** 14.1090 ms

## Architecture Analysis: CPU Cache Efficiency
When calculating a dot product, the processor only needs the X, Y, and Z floats. 
* **AoS Bottleneck:** Data is interleaved (`[XYZ][XYZ]`). When the CPU loads a cache line from RAM, it pulls in the full structure. If a future function only needed the `X` values, loading the `Y` and `Z` data wastes valuable memory bandwidth.
* **SoA Advantage:** Data is strictly contiguous (`[XXX][YYY][ZZZ]`). When the CPU loads a cache line for the `X` array, 100% of the loaded bytes are relevant `X` data, maximizing cache hit rates and compute throughput.

## Scaling to GPU Compute (Memory Coalescing)
While SoA provides a measurable speedup on the CPU, it is a mandatory architectural pattern for GPU programming. GPUs execute instructions using thousands of concurrent threads. 
If 32 threads attempt to read `AoS` data simultaneously, the memory requests are scattered across different addresses, forcing the memory controller to execute multiple serialized transactions. 
By using `SoA`, all 32 threads request contiguous memory addresses simultaneously. The GPU hardware can combine this into a single, massive memory fetch—a process known as **Memory Coalescing**. This prevents memory bandwidth bottlenecks and is critical for scaling a 3D engine.
