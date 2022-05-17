# Modern-CPU-GPU-programming
Welcome! This is a wiki page and code base to support my final project artifacts towards a MS in Computer Science at the University of Tennessee at Chattanooga. These codes were developed using C++ and the Kokkos Ecosystem for the purpose of achieving high performance in a hardware agnostic way via Kokkos parallel abstractions that can enable parallel execution on the CPU and/or the GPU in heterogenous manycore architectures. Heterogeneous parallel programming is essential for Exascale and other high-performance systems, given the realities of modern architectures. Kokkos essentially allows us to write high performance computing applications in a way such that the applications can acheive both performance and portability by compiling and optimizing for the hardware. Without it, one would normally have to rewrite applications anytime they wanted to run their code on another cluster or system with a different programming model/hardware architecture. Instead, we can write code in a way that can achieve performance across theoretically any HPC platform without the need to refactor the code. This saves time as the average HPC application is 300,000-600,000 lines of code, and also makes optimizing the memory access patterns between diverse devices like CPUs and GPUs for the best performance easier.

In my experience, C++ and Kokkos present a strong case for establishing a more unified approach to writing HPC applications as modern memory architectures continue to become more and more diverse.  

# Getting started 
**All you need is a C++ Compiler and Cmake** (but its more fun if you have OpenMP and Cuda too). 
At the time of writing this, I was using: 
* gcc/10.2.0 (with OpenMP 4.5)
* cmake/3.19.4
* cuda/11.3

on a compute cluster node with 80 logical cores and four NVIDIA GPUs.

## Build Instructions

1). Start by cloning the Kokkos Repository. I like doing this in a folder like ~/installs, but if you want to be extra safe, clonde directly to $HOME via

    cd ###
    git clone https://github.com/kokkos/kokkos.git

2) Now we need to build the library. Do this via: 

### For Building the Serial Backend: 
    
    mkdir build && cd build 
    cmake .. -DCMAKE_INSTALL_PREFIX=<PAth =-to-where-you-cloned-Kokkos-in-step-1> 
             -DCMAKE_CXX_COMPILER=<path-to-your-g++> 
             
### For building with OpenMP Enabled 
    
    cmake .. -DCMAKE_INSTALL_PREFIX=<path-to-where-you-cloned-Kokkos-in-step-1>
             -DCMAKE_CXX_COMPILER=<path-to-your-g++>
             -DKokkos_ENABLE_OPENMP=ON 
              
### For building with CUDA Enabled  
             
    cmake .. -DCMAKE_INSTALL_PREFIX=<path-to-where-you-cloned-Kokkos-in-step-1> 
             -DCMAKE_CXX_COMPILER=kokkos/bin/nvcc_wrapper 
             -DKokkos_ENABLE_CUDA=ON 
                      
### Recommended build with advanced optimizations: Here I'm optimizing for NVIDIA VOLTA 
    cmake .. -DCMAKE_INSTALL_PREFIX=<path-to-where-you-cloned-Kokkos-in-step-1>
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
         
6) Run the Exe the CMakeLists.txt to make to build.  
         
            ./<exename>    
            
7) Optional Run-time args
   
             export OMP_NUM_THREADS=<#> 
             EXPORT OMP_PROC-BIND=spread
             export OMP_PLACES=threads
             ./<exename> --kokkos-num-devices=4 (if you have 4 GPUs)
             ./<exename> --nkokkos-numa=2   (if you have 2 NUMA regions)
             
Let me know your results or if you need help! tsgorham@outlook.com     
