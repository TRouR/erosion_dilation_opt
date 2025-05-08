# Morphological Erosion and Dilation in Image Processing (Embedded Systems Project)

This project was developed for my course **Embedded Systems** and focuses on the morphological operations 
of **Erosion** and **Dilation** in binary images. It is implemented in C using manual loop-level optimization techniques 
and simulated with the **ARM eXtended Debugger (AXD)**. The goal is to **minimize execution cycles** through **algorithmic and architectural refinements**.

## Repository Structure

```plaintext
erosion_dilation_opt/
├── scripts/                     # C source files & test image
│   ├── Initial.c
│   ├── LoopInterchange.c
│   ├── MoreFusion.c
│   ├── LoopUnroll.c
│   ├── LoopUnroll&MoreFusion(Final).c
│   ├── Fusion.c
│   ├── DataReuse.c             # Task 3: Optimized with buffers
│   ├── cherry_200x200_binary.yuv
│   ├── stack.c
│   └── yuv_viewer.py           # Python script for yuv frame display
│
├── memory_architecture/        # Memory maps & scatter files
│   ├── memory_t2.map           # Task 2 memory map
│   ├── scatter_t2.txt          # Task 2 scatter
│   ├── memory_t3.map           # Task 3 memory map
│   ├── scatter_t3.txt          # Task 3 scatter
│
├── LICENSE
└── README.md
```

## Project Tasks Overview

### Task 1: Erosion & Dilation Algorithm Implementation and Loop Optimization

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
- Result: Reduced instruction count and improved total cycles.

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

✅ Final Optimization reduced total cycles by **357,060** compared to the initial implementation.

### Task 2: Hierarchy of Memory

- In Task 1 we assumed ideal memory: infinite size and 1 cycle for reads/writes(no delays).
- Now we introduced realistic memory timing and layout via memory.map file.
- First observed that Total ROM Size is 11642 bytes (/screenshots/UnRoll(i=i+2)&MoreFusion_Make.png)

Therefore,

**Memory Map Layout:**

```paintext
00000000 00003000 ROM   4 R  1/1 1/1
00003000 00055000 RAM   4 RW 250/50 250/50
00058000 00050000 SRAM  4 RW 10/1  10/1
```

- ROM: Stores code and read-only data.
- RAM: Stores ZI and RW data with slower speed 
- SRAM: Faster RAM for performance.

**Execution Stats**

| Stage   | Instructions | Core_Cycles | S_Cycles | N_Cycles  | I_Cycles | C_Cycles | Wait_Stages |    Total     | True_Idle |
|---------|--------------|-------------|----------|-----------|----------|----------|-------------|-------------|-----------|
| Initial |   3,114,269  |  5,933,759  | 3,055,757| 2,198,766 |  759,838 |     0    |  99,316,436 | 105,330,797 |    462    |
| Final   |   3,114,296  |  5,933,795  | 3,055,788| 2,198,769 |  759,840 |     0    |  3,234,356  |  9,248,753  |    462    |

📉 Massive cycle drop after hierarchy modeling and SRAM use.

## Task 2: Data Reuse with Buffers

- Goal: Reuse data that is used very frequently to reduce the number of accesses to main memory, thus reducing the number of Wait Stages and the Total Cycles of the program.
- We want to save delays from reading in main memory.
- Used **3 row buffers** to reuse parts of the input matrix instead of reading from memory multiple times.

### Results

| Version          | Instructions | Core_Cycles | S_Cycles  | N_Cycles  | I_Cycles | C_Cycles | Wait_Stages  |    Total     | True_Idle |
|------------------|--------------|-------------|-----------|-----------|----------|----------|--------------|--------------|-----------|
| No Buffer Reuse  |  3,114,295   |  5,933,803  | 3,055,775 | 2,198,790 | 759,842  |     0    | 99,317,924   | 105,332,331  |    462    |
| Buffer in RAM    |  3,617,301   |  7,001,022  | 3,438,563 | 2,722,105 | 921,156  |     0    | 139,186,528  | 146,268,352  |    462    |
| Buffer in SRAM   |  3,617,328   |  7,001,058  | 3,438,594 | 2,722,108 | 921,158  |     0    | 43,355,368   | 50,437,228   |    462    |
