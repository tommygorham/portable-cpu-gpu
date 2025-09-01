/* Purpose: Add a moderately large vector to watch AMD GPU being actively utilized
* Note: 
* Increasing N: Allocates more VRAM and potentially keep the GPU busier for longer. Be mindful of your GPU's memory limits.
* Increasing numIterations: Makes the loop run longer to observe the effect on rocm-smi.
* Changing blockSize: 256 is a good general starting point, but the optimal blockSize can vary depending on your specific GPU architecture. It influences how threads are grouped and executed.
* Why the loop?: The loop around the kernel launch ensures a sustained load so rocm-smi can catch the peak utilization */ 
#include <iostream>
#include <vector>
#include <hip/hip_runtime.h>

// A simple error checking macro for HIP API calls
#define HIP_CHECK(call)                                                          \
do {                                                                             \
    hipError_t err = call;                                                       \
    if (err != hipSuccess) {                                                     \
        std::cerr << "HIP Error at " << __FILE__ << ":" << __LINE__             \
                  << " - " << hipGetErrorString(err) << std::endl;             \
        exit(EXIT_FAILURE);                                                      \
    }                                                                            \
} while (0)

// Kernel to perform vector addition on the GPU
__global__ void vectorAdd(float* A, float* B, float* C, int N) {
    // Calculate global index for the current thread
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Check if the current index is within the vector bounds
    if (idx < N) {
        C[idx] = A[idx] + B[idx];
    }
}

int main() {
    const int N = 1024 * 1024 * 16; // 16 million elements - a moderately large vector
    const int arrayBytes = N * sizeof(float);

    // 1. Host (CPU) vectors
    std::vector<float> h_A(N);
    std::vector<float> h_B(N);
    std::vector<float> h_C(N); // To store results from device

    // Initialize host vectors
    for (int i = 0; i < N; ++i) {
        h_A[i] = static_cast<float>(i);
        h_B[i] = static_cast<float>(i * 2);
    }

    // 2. Device (GPU) pointers
    float *d_A, *d_B, *d_C;

    // Allocate memory on the device
    HIP_CHECK(hipMalloc(&d_A, arrayBytes));
    HIP_CHECK(hipMalloc(&d_B, arrayBytes));
    HIP_CHECK(hipMalloc(&d_C, arrayBytes));

    // 3. Copy input vectors from host to device
    HIP_CHECK(hipMemcpy(d_A, h_A.data(), arrayBytes, hipMemcpyHostToDevice));
    HIP_CHECK(hipMemcpy(d_B, h_B.data(), arrayBytes, hipMemcpyHostToDevice));

    // Determine grid and block dimensions for kernel launch
    // A common block size is 256 or 512 threads
    const int blockSize = 256;
    int numBlocks = (N + blockSize - 1) / blockSize; // Ceiling division

    std::cout << "Launching vectorAdd kernel with " << numBlocks << " blocks and "
              << blockSize << " threads per block, for " << N << " elements." << std::endl;

    // 4. Launch the kernel
    // Run this in a loop to keep the GPU busy for a visible duration
    const int numIterations = 1000; // Run many iterations
    for (int i = 0; i < numIterations; ++i) {
        vectorAdd<<<numBlocks, blockSize>>>(d_A, d_B, d_C, N);
        // Check for kernel launch errors immediately
        HIP_CHECK(hipGetLastError());
    }

    // 5. Synchronize the device and check for any remaining errors after the loop
    // This will block the CPU until all iterations are complete
    HIP_CHECK(hipDeviceSynchronize());
    std::cout << "Kernel execution complete after " << numIterations << " iterations." << std::endl;

    // 6. Copy result from device to host (optional, but good for verification)
    HIP_CHECK(hipMemcpy(h_C.data(), d_C, arrayBytes, hipMemcpyDeviceToHost));

    // Basic verification of results 
    bool passed = true;
    for (int i = 0; i < 10; ++i) { // Check first 10 elements
        if (h_C[i] != (h_A[i] + h_B[i])) {
            std::cerr << "Mismatch at index " << i << ": " << h_C[i] << " != "
                      << h_A[i] << " + " << h_B[i] << std::endl;
            passed = false;
            break;
        }
    }
    if (passed) {
        std::cout << "Verification passed for first 10 elements." << std::endl;
    }

    // 7. Free device memory
    HIP_CHECK(hipFree(d_A));
    HIP_CHECK(hipFree(d_B));
    HIP_CHECK(hipFree(d_C));

    std::cout << "Program finished successfully." << std::endl;

    return 0;
}
