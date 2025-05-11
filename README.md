# 📟 Morphological Erosion and Dilation in Image Processing (Embedded Systems Project)

This project was developed for my course **Embedded Systems** in Electrical & Computer Engineering and focuses on the morphological operations 
of **Erosion** and **Dilation** in binary images. It is implemented in C using manual loop-level optimization techniques 
and simulated with the **ARM eXtended Debugger (AXD)**. The goal is to **minimize execution cycles** through **algorithmic and architectural refinements**.

## 🟠 Repository Structure

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
│   ├── stack.c                 # Stack/heap memory layout
│   └── yuv_viewer.py           # Python script for displaying yuv frames
│
├── memory_architecture/        # Memory maps & scatter files
│   ├── memory_t2.map           # Task 2 memory map
│   ├── scatter_t2.txt          # Task 2 scatter file
│   ├── memory_t3.map           # Task 3 memory map
│   ├── scatter_t3.txt          # Task 3 scatter file
│
├── screenshots/                # AXD simulation screenshots   
│   └── ...
|
├── LICENSE
└── README.md
```

## 🟠 Project Tasks Overview

### ֎ Task 1: Erosion & Dilation Algorithm Implementation and Loop Optimizations

**→ Erosion:**

- A 2x2 structuring element is slid over the image.
- If **all** pixels under the structuring element are 255, the seed point of the elemnt (output pixel) becomes 1; else, 0.

**→ Dilation:**

- If **at least one** pixel under the element is 255, the output pixel becomes 1; else, 0.

#### ➡ Optimization Applied

**① External Padding (Initial)**
- Moved padding logic outside the erosion and dilation functions to avoid boundary checks within the main loops.
- This eliminates conditional statements for every pixel during processing.
- Result: fewer instructions executed per iteration, leading to reduced total cycles and improved performance.

**② Loop Interchange**
- Modified the loop nesting to traverse the image column-wise instead of the default row-wise order.
- Since C stores 2D arrays in row-major format, this led to non-sequential memory access patterns.
- As a result, the processor frequently encountered branch instructions and could not efficiently prefetch data, leading to pipeline stalls and increased total cycles.

**③ Loop Fusion**
- Merged separate for loops in the padding routines into a single loop to reduce control overhead.
- Result: Reduced instruction count and improved total cycles.

**④ Loop Unrolling**
- Unrolled the outer loop (i) in the erosion and dilation functions using a step size of 2 (i += 2).
- This reduced the number of loop control instructions (branching and comparisons), decreasing overall instruction count.
- Tried larger steps but saw diminishing returns due to code bloat.

✅ **Final Optimization**: Loop Unroll + Loop Fusion. Reduced total cycles by **357,060** compared to the initial implementation.

#### ➡ Results Table (AXD Debugger)

| Optimization           | Instructions | Core_Cycles | S_Cycles | N_Cycles | I_Cycles | C_Cycles | Total_Cycles |
|------------------------|--------------|-------------|----------|----------|----------|----------|--------------|
| Initial                | 3,263,122    | 6,290,241   | 3,204,195| 2,365,000| 802,250  | 0        | 6,371,445    |
| Loop Interchange       | 3,501,117    | 6,527,484   | 3,442,219| 2,364,620| 881,449  | 0        | 6,688,288    |
| More Fusion            | 3,256,680    | 6,277,773   | 3,197,763| 2,360,170| 800,642  | 0        | 6,358,575    |
| Loop Unroll            | 3,120,724    | 5,946,251   | 3,062,201| 2,203,602| 761,452  | 0        | 6,027,255    |
| Loop Unroll + Fusion   | 3,114,282    | 5,933,783   | 3,055,769| 2,198,772| 759,844  | 0        | 6,014,385    |

### ֎ Task 2: Memory Hierarchy Modeling

- In Task 1, the implementation assumed an ideal memory model, that has infinite capacity with 1-cycle read/write access time and no latency.
- Task 2 introduces a realistic memory hierarchy by defining timing and structure through a custom memory.map file.
- Upon compilation, the system reported a Total ROM size of 11,642 bytes (as seen in UnRoll(i=i+2)&MoreFusion_Make.png).
  
#### ➡ Memory Map Layout:

```paintext
00000000 00003000 ROM   4 R  1/1 1/1
00003000 00055000 RAM   4 RW 250/50 250/50
00058000 00050000 SRAM  4 RW 10/1  10/1
```

- **ROM**: Stores code and read-only data.
- **RAM**: Stores zero-intialized (ZI) and read-write (RW) data but incurs higher access latency.
- **SRAM**: A fast memory segment placed close to the processor, allocated for performance-critical data.

#### ➡ Execution Stats

| Stage   | Instructions | Core_Cycles | S_Cycles | N_Cycles  | I_Cycles | C_Cycles | Wait_Stages | Total_Cycles | True_Idle |
|---------|--------------|-------------|----------|-----------|----------|----------|-------------|--------------|-----------|
| Initial |   3,114,269  |  5,933,759  | 3,055,757| 2,198,766 |  759,838 |     0    |  99,316,436 | 105,330,797  |    462    |
| Final   |   3,114,296  |  5,933,795  | 3,055,788| 2,198,769 |  759,840 |     0    |  3,234,356  |  9,248,753   |    462    |

- **Initial**: ZI data in RAM, resulting in substantial wait stages.
- **Final**:   ZI data in SRAM, significantly reducing memory latency and total execution cycles (very expensive).

📉 This optimization resulted in a dramatic drop in both **wait stages** and **overall execution time**, 
    highlighting the effectiveness of utilizing fast local memory for frequently accessed data.
  
### ֎ Task 3: Data Reuse

- The objective here is to minimize memory access latency by reusing frequently accessed data (reducing Wait Stages and Total Execution Cycles).
- To achieve this, the implementation introduces three buffers arrays to cache rows of the input matrix in fast SRAM memory.

#### ➡ Key Concept

- Rather than fetching three new rows from RAM during each iteration, the system loads the first three rows into buffer1, buffer2, and buffer3.
- Only two new rows are fetched on each pass, while one buffer is retained from the previous iteration.
  
In this way redundant reads from slow RAM are reduced and fast small SRAM is used to improve memory locality.

#### ➡ Memory Map Layout:

```paintext
00000000 00003000 ROM 4 R 1/1 1/1
00003000 00055000 RAM 4 RW 250/50 250/50
00058000 00002000 SRAM 4 RW 10/1 10/1
```

#### ➡ Results

| Version          | Instructions | Core_Cycles | S_Cycles  | N_Cycles  | I_Cycles | C_Cycles | Wait_Stages  | Total_Cycles | True_Idle |
|------------------|--------------|-------------|-----------|-----------|----------|----------|--------------|--------------|-----------|
| No Buffer Reuse  |  3,114,295   |  5,933,803  | 3,055,775 | 2,198,790 | 759,842  |     0    | 99,317,924   | 105,332,331  |    462    |
| Buffers in RAM   |  3,617,301   |  7,001,022  | 3,438,563 | 2,722,105 | 921,156  |     0    | 139,186,528  | 146,268,352  |    462    |
| Buffers in SRAM  |  3,617,328   |  7,001,058  | 3,438,594 | 2,722,108 | 921,158  |     0    | 43,355,368   | 50,437,228   |    462    |

- Storing buffer data in slow RAM introduces excessive overhead due to repeated accesses to high-latency memory.
- By relocating the buffers to fast SRAM, access time is minimized and memory locality is improved.
  
📉 This optimization results in a ~52% reduction in total cycles compared to the unoptimized case, demonstrating the effectiveness of data reuse when paired with fast memory.

## 🟠 Technical Notes & Definitions

### 🚩 Cycle Types

- **Instructions:** Assemble commands.
- **Core_Cycles:** Total number of cycles spent by the processor executing instructions. This includes sequential, non-sequential, and internal cycles, but excludes wait states.
- **S_Cycles (Sequential Cycles):**
  Cycles during which memory accesses occur in a predictable, linear fashion. The Program Counter (PC) increments by 1, enabling efficient instruction prefetching and memory pipelining.
- **N_Cycles (Non-Sequential Cycles):**
  Occur when the PC performs jumps or branches. These accesses are harder to predict, leading to slower memory fetches and more Wait_Stages.
- **I_Cycles (Internal Cycles):**
  Execution cycles that do not require memory access. These include internal operations like arithmetic or logic, fully processed within the CPU.
- **C_Cycles (Co-processor Cycles):**
  Always zero in this project. These cycles would be present only if an extra co-processor were active.
- **Wait_Stages:**
  Stall cycles incurred when memory access is delayed. These are a major contributor to total execution time.
- **True_Idle:** Cycles during which the processor is idle and not executing any instructions.
  
### 📍 Data Types

- **RO Data (Read-Only):** Constants and program code stored in ROM.
- **RW Data (Read-Write):** Initialized variables stored in RAM.
- **ZI Data (Zero-Initialized):** Uninitialized variables, typically placed in RAM.

### 📍 Loop Transformations

- **Loop Unrolling:**
  Increases loop step size (e.g., i += 2) to reduce loop control overhead (fewer compare, branch, etc.). Improves performance at the cost of increased code size.
- **Loop Fusion:**
  Combines multiple loops into a single loop to eliminate redundant iteration logic, reducing total instruction count.
- **Loop Interchange:**
  Swaps nested loop order to change memory access patterns. When misaligned with memory layout (e.g., scanning column-wise in a row-major language like C), this leads to non-sequential memory access and degraded 
  performance.

### 📍 Memory Layout & Tooling

- **Memory Hierarchy (memory.map):**
  Specifies memory regions (ROM, RAM, SRAM), bus width, access permissions, and access times (e.g., 250/50 ns).

- **Scatter File:** Defines the areas of memory where the basic data for the operation of the system are stored. 

- **Stack/Heap Configuration:**
  Stack (local variables store) grows upward, heap downward, both managed within RAM. Their locations and limits are manually configured in stack.c.

## 🟠 Simulation Setup

- **Debugger:** AXD (ARM eXtended Debugger)
- **Toolchain:** ARM Developer Suite (ADS)
- **Target:** ARM7TDMI
-	**Clock:** 50 MHz (Emulated)
- **Endianness:** Little Endian
- **FPU/MMU:** No FPU, default pagetables
