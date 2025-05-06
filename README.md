# Morphological Erosion and Dilation in Image Processing

This project was developed for my course **Integrated Systems** and focuses on the morphological operations 
of **Erosion** and **Dilation** in binary images. It is implemented in C using manual loop-level optimization techniques 
and simulated with the **ARM eXtended Debugger (AXD)**. The goal is to minimize execution cycles through algorithmic and architectural refinements.

## Project Tasks Overview
### Task 1: Erosion & Dilation Algorithm Implementation
**Erosion:**

- A 2x2 structuring element is slid over the image.

- If **all** pixels under the structuring element are 1 (or 255), the output pixel becomes 1; else, 0.

**Dilation:**

- If **at least one** pixel under the element is 1 (or 255), the output pixel becomes 1; else, 0.

### Optimization Applied
**1. External Padding Optimization**
- Moved the boundary checks outside the main erosion/dilation loops by explicitly padding the input.
- Reduced if conditions per pixel => lowered instruction count and total cycles.

**2. Loop Interchange**
- Changed access pattern from row-major to column-major.
- However, since C uses row-major storage, this increased non-sequential accesses and total cycles.

**3. Loop Fusion**
- Combined multiple for loops into single iterations in padding routines.
- Result: Reduced instruction count and slightly improved total cycles.

**4. Loop Unrolling**
- Unrolled the i loop in erosion/dilation by a step of 2 (i += 2).
- Reduced branch and compare instructions => fewer iterations.
- Tried larger steps but saw diminishing returns due to code bloat.

🔄 **Final Combination**
- Used both Loop Unrolling and Fusion for optimal performance.

### 📋 Results Table (AXD Debugger)

| Optimization           | Instructions | Core_Cycles | S_Cycles | N_Cycles | I_Cycles | C_Cycles | Total     |
|------------------------|--------------|-------------|----------|----------|----------|----------|-----------|
| Initial                | 3,263,122    | 6,290,241   | 3,204,195| 2,365,000| 802,250  | 0        | 6,371,445 |
| Loop Interchange       | 3,501,117    | 6,527,484   | 3,442,219| 2,364,620| 881,449  | 0        | 6,688,288 |
| More Fusion            | 3,256,680    | 6,277,773   | 3,197,763| 2,360,170| 800,642  | 0        | 6,358,575 |
| Loop Unroll            | 3,120,724    | 5,946,251   | 3,062,201| 2,203,602| 761,452  | 0        | 6,027,255 |
| Loop Unroll + Fusion   | 3,114,282    | 5,933,783   | 3,055,769| 2,198,772| 759,844  | 0        | 6,014,385 |

✅ Task 2: Hierarchy of Memory

- Initially assumed ideal memory: 1 cycle for reads/writes.
- Introduced realistic memory hierarchy via memory.map file.


