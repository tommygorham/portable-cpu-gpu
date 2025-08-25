# Modern-CPU-GPU-programming
A collection of codes, insights, and performance portability framework experiments for cpu + gpu programming 

## Directory structure
`kokkos`: C++ codes using a performance portability framework for CPU+GPU parallel execution

# Context 
### THE CPU (Central Processing Unit) 
E.g., AMD Ryzen 9 7950X **(16 physical cores, 32 logical cores)**
* **What:** Used for sequential tasks & complex decision-making
* **How:** Fewer, more powerful cores focused on complex tasks (e.g., if -> then) 
* **Why:** It's like the brain of the computer, orchestrating what happens when
* Advanced Control Logic, optimized for low latency 
* Big caches (L1, L2, L3), some processors may share L2   

### CPU Optimizations 
* Keep variables close to cache, as this is much faster to access than main memory
(E.g., padding the array to cache line)
* Access matrices row-major order, and keep abstraction to 1D (i*N+j) 
* Advanced Vector Instructions/Compiler Intrinsics (SSE, AVX2, -O2, SMT, etc..)
* OpenMP shared memory parallelism, cyclic distribution
* MPI for distributed memory parallelism, data communication
* [My favorite resource:](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf) 

### THE GPU (Graphics Processing Unit)  
E.g., NVIDIA RTX 4090 **(16,384 cores)** 
* **What:** Used for graphics rendering images & machine learning
* **How:** Thousands of simple cores optimized for parallel processing
* **Why:** Having many more cores than the CPU make the GPU much faster at large-scale, simple calculations
* SIMT, data parallelism: Groups of GPU cores perform exact same operations at the exact same time 
* Advantages over the CPU include high throughput and high memory bandwith 

``` c++/cuda code
// cuda kernel launch <<< num_blocks, num_threads_per_block >>> 
computeOnGPU <<< 50, 1024 >>>
// 50 * 1024 = 51,200 logical threads ... You can't launch 51,200 threads on a CPU 
```
* **CUDA Threads:** logical threads created by the GPU when you launch a kernel. Each thread executes a portion of the code on the GPU.
* **CUDA Blocks:** groups of threads. Threads within a block can communicate with each other and share resources, but blocks are independent and cannot communicate with each other.
The number of blocks and threads per block you define determines how the work is divided among the GPU’s cores.

### GPU Optimizations 
* Coalesce memory access to minimize the time the program spends in memory access 
* Fully saturate all the GPU cores if possible 
* Note: The GPU cores are not directly mapped to threads one-to-one. Instead, CUDA organizes threads into warps (groups of 32
threads) and schedules them across available cores.
* Number of threads per block that's a multiple of 32.
* Aim to have enough blocks to cover all the GPU cores.
(i.e., numblocks * numthreadsperblock = cuda cores)  
* Profiling tools: nvidia-smi, rocm-smi, nvprof, nvidia nsight [this
* calculator](https://view.officeapps.live.com/op/view.aspx?src=https%3A%2F%2Fdocs.nvidia.com%2Fcuda%2Fcuda-occupancy-calculator%2FCUDA_Occupancy_Calculator.xls&wdOrigin=BROWSELINK)

[Wiki](https://github.com/tommygorham/modern-cpu-gpu-programming/wiki) 
