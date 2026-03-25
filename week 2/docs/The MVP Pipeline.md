# 3D Transform Pipeline

## Homogeneous Division & Clip-Space Mapping
*(Fulfills Syllabus Mastery Check 3)*

To project 3D coordinates onto a 2D screen, the engine uses a 4D vector: $[X, Y, Z, W]$. 

During the Projection Matrix multiplication, the original $Z$ depth of the vertex is mathematically copied into the $W$ slot (creating the "W-Trap"). 

To map the infinite 3D universe into the GPU's Normalized Device Coordinates (the $[-1, 1]$ clip-space cube), the hardware performs the **Perspective Divide**: it divides the transformed $X$, $Y$, and $Z$ components by the new $W$ component. Because $W$ scales with distance, objects further away are divided by a larger number. This effectively shrinks their $X$ and $Y$ coordinates, creating the physical illusion of perspective depth on a flat 2D monitor.
