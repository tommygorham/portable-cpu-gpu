#include <iostream>
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

int main() {
    int deviceCount;
    HIP_CHECK(hipGetDeviceCount(&deviceCount));

    if (deviceCount == 0) {
        std::cerr << "No HIP devices found." << std::endl;
        return 1;
    }

    std::cout << "Found " << deviceCount << " HIP device(s)." << std::endl;

    // Iterate through all found devices
    for (int dev = 0; dev < deviceCount; ++dev) {
        hipDeviceProp_t props;
        HIP_CHECK(hipGetDeviceProperties(&props, dev));

        std::cout << "\n--- Device " << dev << ": " << props.name << " ---" << std::endl;
        // hipDeviceProp_t may not have gcnArch, if next line errors out, try props.arch  
        //std::cout << "  Compute Capability (arch): " << props.gcnArch << std::endl;
        std::cout << "  Compute Units (CUs):       " << props.multiProcessorCount << std::endl;
        std::cout << "  Global Memory:             " << props.totalGlobalMem / (1024 * 1024) << " MB" << std::endl;
        std::cout << "  Shared Memory per Block:   " << props.sharedMemPerBlock / 1024 << " KB" << std::endl;
        std::cout << "  Registers per Block:       " << props.regsPerBlock << std::endl;
        std::cout << "  Warp Size (Wavefront Size):" << props.warpSize << " threads" << std::endl;
        std::cout << "  Max Threads per Block:     " << props.maxThreadsPerBlock << std::endl;
        std::cout << "  Max Block Dimensions:      (" << props.maxThreadsDim[0] << ", "
                  << props.maxThreadsDim[1] << ", " << props.maxThreadsDim[2] << ")" << std::endl;
        std::cout << "  Max Grid Dimensions:       (" << props.maxGridSize[0] << ", "
                  << props.maxGridSize[1] << ", " << props.maxGridSize[2] << ")" << std::endl;
        std::cout << "  Clock Rate:                " << props.clockRate / 1000 << " MHz" << std::endl;
        std::cout << "  Memory Clock Rate:         " << props.memoryClockRate / 1000 << " MHz" << std::endl;
        std::cout << "  Bandwidth (approx):        " << (props.memoryBusWidth / 8 * props.memoryClockRate / 1000 / 1000) << " GB/s" << std::endl;
        // This is using props.maxTexture1D as a proxy for the maximum allocatable memory. This is a common trick used
        // in CUDA and HIP examples because there isn't always a direct maxAllocationSize member that reflects the
        // absolute largest single allocation you can make outside of textures. The most reliable way to know the total
        // available global memory is props.totalGlobalMem. For the largest single contiguous allocation, there isn't
        // always a direct property. If Max Memory Allocation line shows 0, try this: 
        // std::cout << "  Global Memory:             " << props.totalGlobalMem / (1024 * 1024) << " MB" << std::endl;
        std::cout << "  Max memory allocation:     " << props.maxTexture1D / (1024 * 1024) << " MB (maxTexture1D used as proxy)" << std::endl;
        std::cout << "  ECC Enabled:               " << (props.ECCEnabled ? "Yes" : "No") << std::endl;
        std::cout << "  Integrated GPU:            " << (props.integrated ? "Yes" : "No") << std::endl;
    }

    return 0;
}
