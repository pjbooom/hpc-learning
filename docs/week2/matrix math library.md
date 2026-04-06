# Matrix Mathematics Library

## GPU Matrix Representation Note
*(Fulfills Syllabus Micro-task 5)*

C++ naturally stores 2D arrays in **Row-Major** order (reading left-to-right, row by row). However, graphics APIs like OpenGL expect matrix data in **Column-Major** order (reading top-to-bottom, column by column). 

To bridge this gap, our `Mat4` struct uses a flat, 1D `float m[16]` array mathematically mapped to Column-Major format. Memory slots `m[0]` through `m[3]` physically represent the first column. This ensures that when we pass our matrix pointer directly to the GPU shader, the memory alignment perfectly matches the hardware's expectations, avoiding the need for an expensive CPU transpose operation right before rendering.