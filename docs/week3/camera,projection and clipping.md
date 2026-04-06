# Week 3: Camera, Projection & Clipping
**Date:** March 15–21, 2026  
**Objective:** Implement camera view (lookAt), perspective projection, and near-plane clipping.

---

## 1. Camera View Transformation (`lookAt`)
I implemented a robust `lookAt` function in `include/math/camera.h` to define the camera's position and orientation in 3D space.

* **Logic:** The function calculates a coordinate system (basis vectors) for the camera:
    * **Forward:** Normalized vector from `eye` to `target`.
    * **Right:** Cross product of the world `up` and the `forward` vector.
    * **Up:** Cross product of `forward` and `right`.
* **Validation:** Verified by rendering the scene from three distinct poses:
    1.  **Isometric:** `eye(3, 3, 3)`, `target(0, 0, 0)`
    2.  **Top-Down:** `eye(0, 5, 0)`, `target(0, 0, 0)`
    3.  **Side-Profile:** `eye(5, 0, 0)`, `target(0, 0, 0)`

> [!TIP]
> **Mastery Check #1:** Perspective is correctly reproduced across all camera poses without distortion.

---

## 2. Perspective Projection & FOV
To transition from orthographic to perspective viewing, I implemented a perspective projection matrix.

* **Field of View (FOV):** Experimented with FOV values. 
    * At **90°**, the scene feels natural. 
    * At **120°**, the "wide-angle" effect is visible, stretching objects at the edges.
* **Aspect Ratio:** Calculated as `width / height` (e.g., 1.33 for 800x600) to prevent the cube from looking squashed.

---

## 3. Sutherland–Hodgman Near-Plane Clipping
**Mastery Check #2:** To handle triangles that are partially behind the camera, I implemented a clipper for the near plane.

* **Implementation:** If an edge crosses the `z_near` plane, a new vertex is interpolated at the intersection point.
* **Results:** This prevents "divide by zero" errors during the perspective divide and ensures the rasterizer only processes geometry within the visible frustum.

---

## 4. Depth Precision & Z-Buffer Experiment
**Mastery Check #3:** I recorded the impact of the `near` and `far` plane distances on the Z-buffer.

| Near Plane | Far Plane | Observation |
| :--- | :--- | :--- |
| 0.0001 | 10000.0 | **Severe Z-Fighting:** Distant faces flicker because the floating-point precision is wasted on the space too close to the camera. |
| 0.1 | 100.0 | **Stable Depth:** Geometry renders cleanly with no overlapping artifacts. |

**Conclusion:** A larger `near` plane value (e.g., 0.1) is essential for maintaining depth buffer stability in a software renderer.

---

