# THE GPU (Graphics Processing Unit)  
* Responsible for generating and rendering images to the screen
* Architecture is designed for high throughput 
* This implies many threads per core = more performance 
* In essence, the GPU does the same thing as CPU (e.g., add two numbers) 
* However, The GPU many more cores than the CPU 
``` c++ 
// E.g., cuda kernel launch <<< num_blocks, num_threads_per_block >>> 
computeOnGPU <<< 50, 1024 >>>
// 50 * 1024 = 51,200 threads 
// You probably can't launch 51,200 threads on a CPU 
```
* SIMT, data parallelism: Groups of GPU cores perform exact same operations at the exact same time 
* Advantages over the CPU include high throughput and high memory bandwith 

# THE CPU 
* Advanced Control Logic 
* Designed to reduce waiting for input time
* This implies it's optimized for low latency 
* Big caches (L1, L2, L3), some processors may share L2   
* Keep variables close to cache,  much faster to access than main memory

# General Optimizations  
* reduce the number of operations if possible (e.g., domain decomposition) 
* reduce communication overhead 
* increase effeciency of code 
``` python
efficiency =  # ops performed / max # ops 
def calcGPUProgramEfficiency(): 
    #assuming both numerator and denoninator are gigaflops 
    # numerator for efficiency equation
    N = float(input("Enter number of instructions the program executes\n(E.g., size of array): ")) 
    # vars for denominator 
    num_cores = float(input("Enter the number of GPU Cores: "))
    ghz = float(input("Enter the GPU's base frequency: ")) 
    N_per_clock = float(input("Enter the number of ops per clock: "))
    E = N / (num_cores * ghz * N_per_clock)
    print("\nEfficiency: ")
    print(round(E, 3)) 

def main(): 
    input = calcGPUProgramEfficiency() 
main() 
```
# CPU Optimizations 
* Use Advanced Vector Instructions/Compiler Intrinsics (SSE, AVX2, -O2, etc..)
* OpenMP shared memory parallelism (without false sharing, pay attention to processor cache map)
  - can implement cyclic distribution (e.g., padding the array to cache line)
* Access matrices row-major order, and keep abstraction to 1D (i*N+j) 
* Read (this)[https://people.freebsd.org/~lstewart/articles/cpumemory.pdf]  and then read it again

# GPU Optimizations 
* Feed the GPU compute intensive, parallel computations
* For the GPU, implement fine-grained parallelism
* Coalesce memory access to minimize the time the program spends in memory access 
* Fully saturate all the GPU cores if possible 
* e.g., nvidia nsight is helpful
* So is (this calculator)[https://view.officeapps.live.com/op/view.aspx?src=https%3A%2F%2Fdocs.nvidia.com%2Fcuda%2Fcuda-occupancy-calculator%2FCUDA_Occupancy_Calculator.xls&wdOrigin=BROWSELINK]
