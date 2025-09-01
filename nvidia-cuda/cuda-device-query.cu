#include <iostream>
#include <cuda_runtime.h> // Essential for CUDA Runtime API functions and structs

// A simple error checking macro for CUDA API calls
#define CUDA_CHECK(call)                                                         \
do {                                                                             \
    cudaError_t err = call;                                                      \
    if (err != cudaSuccess) {                                                    \
        std::cerr << "CUDA Error at " << __FILE__ << ":" << __LINE__            \
                  << " - " << cudaGetErrorString(err) << std::endl;            \
        exit(EXIT_FAILURE);                                                      \
    }                                                                            \
} while (0)

int main() {
    int deviceCount;
    CUDA_CHECK(cudaGetDeviceCount(&deviceCount));

    if (deviceCount == 0) {
        std::cerr << "No CUDA devices found." << std::endl;
        return 1;
    }

    std::cout << "Found " << deviceCount << " CUDA device(s)." << std::endl;

    // Iterate through all found devices
    for (int dev = 0; dev < deviceCount; ++dev) {
        cudaDeviceProp props;
        CUDA_CHECK(cudaGetDeviceProperties(&props, dev));

        std::cout << "\n--- Device " << dev << ": " << props.name << " ---" << std::endl;

        // Mimicking HIP's architecture/compute capability output
        // For NVIDIA, this is typically major.minor for compute capability
        std::cout << "  Compute Capability (arch): " << props.major << "." << props.minor << std::endl;
        std::cout << "  Streaming Multiprocessors (SMs): " << props.multiProcessorCount << std::endl;
        std::cout << "  Global Memory:             " << props.totalGlobalMem / (1024 * 1024) << " MB" << std::endl;
        std::cout << "  Shared Memory per Block:   " << props.sharedMemPerBlock / 1024 << " KB" << std::endl;
        std::cout << "  Registers per Block:       " << props.regsPerBlock << std::endl;
        std::cout << "  Warp Size:                 " << props.warpSize << " threads" << std::endl;
        std::cout << "  Max Threads per Block:     " << props.maxThreadsPerBlock << std::endl;
        std::cout << "  Max Block Dimensions:      (" << props.maxThreadsDim[0] << ", "
                  << props.maxThreadsDim[1] << ", " << props.maxThreadsDim[2] << ")" << std::endl;
        std::cout << "  Max Grid Dimensions:       (" << props.maxGridSize[0] << ", "
                  << props.maxGridSize[1] << ", " << props.maxGridSize[2] << ")" << std::endl;
        std::cout << "  Clock Rate (SM Clock):     " << props.clockRate / 1000 << " MHz" << std::endl;
        // CUDA's memoryClockRate is generally not directly exposed like HIP's,
        // but we can approximate memory bandwidth.
        // Assuming DDR, so effective clock rate is 2 * actual, then * bus width / 8 bits per byte
        // For GDDR5/6, effective is 4x for GDDR5 and 2x for GDDR6 vs base clock.
        // Props.memoryClockRate is often given as actual clock, not effective data rate for GDDR.
        // A more accurate bandwidth calculation for modern GPUs is complex and dependent on memory type.
        // We'll use a simpler approximation based on props.memoryBusWidth and props.clockRate for example.
        // However, props.memoryClockRate is not typically available directly in this struct.
        // We will output bus width which is a good indicator.
        std::cout << "  Memory Bus Width:          " << props.memoryBusWidth << " bits" << std::endl;

        // For Max memory allocation, `maxTexture1D` is a common proxy in CUDA too.
        // CUDA also has `l2CacheSize`, etc.
        // cudaMalloc often aligns to specific sizes and usually gives you large contiguous blocks.
        // Let's use maxTexture1D as requested for consistency if it's meaningful for your card.
        // A more reliable way to get max allocation is `cudaMemGetInfo` after allocating.
        std::cout << "  Max 1D Texture Size:       " << props.maxTexture1D / (1024 * 1024) << " MB (as proxy for max alloc)" << std::endl;

        std::cout << "  ECC Enabled:               " << (props.ECCEnabled ? "Yes" : "No") << std::endl;
        std::cout << "  Integrated GPU:            " << (props.integrated ? "Yes" : "No") << std::endl;
        std::cout << "  Async Engines:             " << props.asyncEngineCount << std::endl; // Specific to NVIDIA for overlap
    }

    return 0;
}
