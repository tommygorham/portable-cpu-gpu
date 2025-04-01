# Modern-CPU-GPU-programming
These codes were developed using C++ and the [Kokkos Ecosystem](https://kokkos.org/) for the purpose 
of learning, experimenting with, and benchmarking performance portability frameworks in high-performance computing. 

As [modern memory architectures continue to become more
diverse](https://github.com/tommygorham/modern-cpu-gpu-programming/wiki/Heterogenous-Architectures#top500-comparison-november-2011---november-2021),
it may be useful to implement code in a more portable way such that it can achieve performance on theoretically any HPC platform,
instead of specific APIs for each specific architecture (Nvidia/Cuda, AMD/HIP, Intel/SYCL)   

One way this is achieved by expressing the code that CAN be executed in parallel with [Kokkos parallel
abstractions](https://kokkos.github.io/kokkos-core-wiki/API/core/ParallelDispatch.html). 

This way, parallel execution and memory access patterns can be configured at compile time, without the code having to
change.

# Getting started 
**Requirements:** C++ Compiler, CMake, Kokkos

**Optional:** OpenMP, Nvidia/AMD/Intel GPU Cluster or Machine 

At the time of writing this, I was using: 
* gcc/10.2.0 (with OpenMP 4.5)
* cmake/3.19.4
* cuda/11.3
* [compute cluster node with 80 logical cores and four NVIDIA GPUs.](https://wiki.simcenter.utc.edu/doku.php/clusters:firefly)

# Instructions

[Install Kokkos](https://kokkos.org/kokkos-core-wiki/get-started/building-from-source.html#configuring-and-building-kokkos)

To Tell CMake where you installed Kokkos with 
```
cmake .. -DKokkos_ROOT=<path-to-your-kokkos-install>
```
Or
```
-DCMAKE_INSTALL_PREFIX=<path-to-your-kokkos-install>
```

## Serial CPU   
```
cmake .. 
-DCMAKE_INSTALL_PREFIX=<path-to-your-kokkos-install>
-DCMAKE_CXX_COMPILER=<path-to-g++> 
```

## OpenMP for on-node shared memory parallelism on the CPU
```
cmake .. 
-DCMAKE_INSTALL_PREFIX=<path-to-your-kokkos-install>
-DCMAKE_CXX_COMPILER=<path-to-g++> 
-DKokkos_ENABLE_OPENMP=ON 
```

## OpenMP + NVIDIA GPU(s) 
```
cmake .. 
-DCMAKE_INSTALL_PREFIX=<path-to-your-kokkos-install>
-DCMAKE_CXX_COMPILER=<path-to-kokkos/bin/nvcc_wrapper> 
-DKokkos_ENABLE_OPENMP=ON 
-DKokkos_ENABLE_CUDA=ON
-DKokkos_ARCH_<YOUR_GPU_ARCHITECTURE>=ON 
-DKokkos_ENABLE_CUDA_LAMBDA=ON
-DKokkos_ENABLE_CUDA_UVM=ON
-DKokkos_ENABLE_CUDA_RELOCATABLE_DEVICE_CODE=ON 
```

## Additional Configuration 
```
export OMP_NUM_THREADS=<#> 
EXPORT OMP_PROC-BIND=spread
export OMP_PLACES=threads
./<exename> --kokkos-num-devices=4 (if you have 4 GPUs)
./<exename> --kokkos-numa=2   (if you have 2 NUMA regions)
```

[Additional info](https://github.com/tommygorham/modern-cpu-gpu-programming/wiki) 
