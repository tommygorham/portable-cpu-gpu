# Modern-CPU-GPU-programming
Welcome! This is a wiki page and code base to support my final project artifacts towards a MS in Computer Science at the University of Tennessee at Chattanooga. These codes were developed using C++ and the [Kokkos Ecosystem](https://kokkos.org/) for the purpose of achieving high performance in a hardware agnostic way. This is achieved by expressing the code that you want to execute in parallel with [Kokkos parallel abstractions](https://kokkos.github.io/kokkos-core-wiki/API/core/ParallelDispatch.html). Writing code with these abstractions enables parallel execution on the CPU and/or the GPU in heterogenous manycore architectures. The execution target (e.g., CPU/GPU) is set at compile time via one or multiple of [these parameters](https://kokkos.github.io/kokkos-core-wiki/keywords.html), along with other optimizations, depending on the architecture. 

# Brief Background 
[_Heterogeneous parallel programming_](https://en.wikipedia.org/wiki/Heterogeneous_computing) is essential for [Exascale](https://en.wikipedia.org/wiki/Exascale_computing) and other high-performance systems, given the realities of modern architectures. [Kokkos](https://github.com/kokkos) is a C++ Performance Portability Framework that provides a more unified approach to writing HPC applications. As [modern memory architectures continue to become more and more diverse](https://github.com/tommygorham/modern-cpu-gpu-programming/wiki/Heterogenous-Architectures#top500-comparison-november-2011---november-2021), we can use portability Frameworks like Kokkos to write high performance computing applications in a way such that the applications can acheive both performance and portability by compiling and optimizing for the hardware. Without Kokkos, one would normally have to rewrite applications anytime they wanted to run their code on another cluster or system with a different programming model/hardware architecture. Instead, we can write code in a way that can achieve performance across theoretically any HPC platform without the need to refactor the code. This saves alot of time, as the average HPC application is 300,000-600,000 lines of code. Using Kokkos also makes optimizing the memory access patterns between diverse devices like CPUs and GPUs easier, since the optimizations can be set at compile time. 

# Getting started 
**All you need is a C++ Compiler and Cmake** (but its more fun if you have OpenMP and Cuda too). 
At the time of writing this, I was using: 
* gcc/10.2.0 (with OpenMP 4.5)
* cmake/3.19.4
* cuda/11.3

The code was executed on a [compute cluster node with 80 logical cores and four NVIDIA GPUs.](https://wiki.simcenter.utc.edu/doku.php/clusters:firefly)

# Build Instructions

1). Start by cloning the [Kokkos Repository](https://github.com/kokkos/kokkos). I like doing this in a folder like ~/installs, but if you want to be extra safe, clonde directly to $HOME via

    cd ###
    git clone https://github.com/kokkos/kokkos.git

2) Now we need to build the library. Do this via: 

### For Building the Serial Backend: 
    
    mkdir build && cd build 
    cmake .. -DCMAKE_INSTALL_PREFIX=<path-to-where-you-want-to-install-kokkos> 
             -DCMAKE_CXX_COMPILER=<path-to-your-g++> 
             
### For building with OpenMP Enabled 
    
    cmake .. -DCMAKE_INSTALL_PREFIX=<path-to-where-you-want-to-install-kokkos>
             -DCMAKE_CXX_COMPILER=<path-to-your-g++>
             -DKokkos_ENABLE_OPENMP=ON 
              
### For building with CUDA Enabled  
             
    cmake .. -DCMAKE_INSTALL_PREFIX=<path-to-where-you-want-to-install-kokkos> 
             -DCMAKE_CXX_COMPILER=kokkos/bin/nvcc_wrapper 
             -DKokkos_ENABLE_CUDA=ON 
                      
### Recommended build with advanced optimizations: Here I'm optimizing for NVIDIA VOLTA 
    cmake .. -DCMAKE_INSTALL_PREFIX=<path-to-where-you-want-to-install-kokkos>
             -DKokkos_ENABLE_CUDA_LAMBDA=ON 
             -DKokkos_ENABLE_CUDA=ON
             -DKokkos_ENABLE_CUDAUVM=ON
             -DKokkos_ENABLE_CUDA_RELOCATABLE_DEVICE_CODE=ON
             -DKokkos_ARCH_VOLTA70=ON 
             -DKokkos_ENABLE_CUDA_LAMBDA=ON 

3) When this finishes, run 

              make install

4) The library is now built, we are almost done! Now, cd to a folder where you want your source code and clone my repo!
             
             cd 
             cd your_experiments/ 
             git clone https://github.com/tommygorham/modern-cpu-gpu-programming.git
             cd modern-cpu-gpu-programming 
             
5) Now you can build my programs and run them by cd'ing into PROGRAM<#> and running cmake ../ in the build folder. For example

             cd PROGRAM1/build 
             !cmake 
             make 
         
Note: *!cmake* ensures you build your program with the same Cmake arguments that you built the Kokkos library with. 

6) Run the Exe the CMakeLists.txt to make to build.  
         
            ./<exename>    
            
7) Optional Run-time args
   
             export OMP_NUM_THREADS=<#> 
             EXPORT OMP_PROC-BIND=spread
             export OMP_PLACES=threads
             ./<exename> --kokkos-num-devices=4 (if you have 4 GPUs)
             ./<exename> --kokkos-numa=2   (if you have 2 NUMA regions)
             
Additionally, you can view my [wiki](https://github.com/tommygorham/modern-cpu-gpu-programming/wiki) for more detailed information 
