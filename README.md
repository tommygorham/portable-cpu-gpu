# Modern-CPU-GPU-programming
These codes were developed using C++ and the [Kokkos Ecosystem](https://kokkos.org/) for the purpose of achieving high performance in a hardware agnostic way. 

Scientific simulation codes often contain memory intensive operations that need to be run in parallel or on the GPU. 

One way this is achieved by expressing the code that you want to execute in parallel with [Kokkos parallel
abstractions](https://kokkos.github.io/kokkos-core-wiki/API/core/ParallelDispatch.html). 

Doing so enables scientific codes to execute across a wide range of diversified modern architectures
out-of-the-box, instead of having to rewrite code in a different language in order to execute in parallel
on different execution resources e.g., AMD GPUs (HIP) vs Nvidia GPUs (Cuda)

The execution target is simply set at compile time, without the code having to change. 

# Brief Background 
[_Heterogeneous parallel programming_](https://en.wikipedia.org/wiki/Heterogeneous_computing) is essential for [Exascale](https://en.wikipedia.org/wiki/Exascale_computing) given the realities of modern architectures. 
[Kokkos](https://github.com/kokkos) is a C++ Performance Portability Framework that provides a more unified approach to
writing HPC applications for heterogeneous systems. 

As [modern memory architectures continue to become more
diverse](https://github.com/tommygorham/modern-cpu-gpu-programming/wiki/Heterogenous-Architectures#top500-comparison-november-2011---november-2021),
it may be useful to implement code that can achieve performance across theoretically any HPC platform via configuration of the parallel execution and memory access patterns to maximize optimization at compile time

Ideally, this would remove the need to rewrite/refactor the code for specific hardware resources and architectures. 

Removing the requirement to rewrite/refactor the code is particularly benefecial, as the average HPC application is
often at least 300,000-600,000 lines. 

# Getting started 
**Requirements:** C++ Compiler, CMake, Kokkos

**Optional:** OpenMP, Cuda/HIP 

At the time of writing this, I was using: 
* gcc/10.2.0 (with OpenMP 4.5)
* cmake/3.19.4
* cuda/11.3

The code was executed on a [compute cluster node with 80 logical cores and four NVIDIA GPUs.](https://wiki.simcenter.utc.edu/doku.php/clusters:firefly)

# Instructions

[Install Kokkos](https://kokkos.org/kokkos-core-wiki/get-started/building-from-source.html#configuring-and-building-kokkos)

### Then, build each program in this repo with cmake
```
cd PROGRAM1/build 
!cmake
make 
```
            
### Optional Run-time args
```
export OMP_NUM_THREADS=<#> 
EXPORT OMP_PROC-BIND=spread
export OMP_PLACES=threads
./<exename> --kokkos-num-devices=4 (if you have 4 GPUs)
./<exename> --kokkos-numa=2   (if you have 2 NUMA regions)
```

[Additional info](https://github.com/tommygorham/modern-cpu-gpu-programming/wiki) 
