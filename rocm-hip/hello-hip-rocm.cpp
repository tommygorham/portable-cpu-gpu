#include <iostream>
#include <hip/hip_runtime.h>

// HIP kernel function - executed on the AMD GPU
__global__ void helloWorldKernel() {
    printf("Hello from GPU world (HIP)!\n");
}

int main() {
    // Launch the kernel on the GPU
    helloWorldKernel<<<1, 1>>>(); // <<<gridDim, blockDim>>> 

    // Checking the return value of hipDeviceSynchronize() to avoid compiler warning 
    // 'ignoring return value of function declared with 'nodiscard' attribute [-Wunused-result]' 
    hipError_t err = hipDeviceSynchronize();
    if (err != hipSuccess) {
        std::cerr << "HIP Error: " << hipGetErrorString(err) << std::endl;
        return 1; // Indicate failure
    }
    std::cout << "Hello from CPU world (HIP)!" << std::endl;
    return 0;
}
