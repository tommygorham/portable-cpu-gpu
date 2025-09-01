# HIP/ROCm Programs 
``` 
# Tested with rocm 6.4.0, checks that everything is installed correctly and ready to go 
hipcc hello-hip-rocm.cpp -o hello-hip-rocm
# Use with command: watch -n 1 rocm-smi 
hipcc hip-vec-add.cpp 
# Most impt
hipcc hip-device-query.cpp 
```

## Using hip-device-query.cpp 
Use These Limits for Dynamic Kernel Launches:

Example: Adapting blockSize and numBlocks for vector addition, Instead of hardcoding blockSize = 256, you could:

``` 
// ... (inside main, after getting props)

    // Example: Dynamically determine block size and grid size
    const int N = 1024 * 1024 * 16;
    const int arrayBytes = N * sizeof(float);

    // Choose a blockSize that is a multiple of warpSize and less than maxThreadsPerBlock
    int blockSize = props.warpSize; // Start with wavefront size
    while (blockSize * 2 <= props.maxThreadsPerBlock && blockSize * 2 <= 1024) { // Cap at 1024 for practical reasons
        blockSize *= 2;
    }
    // Ensure blockSize is not 0 in case of very weird hardware
    if (blockSize == 0) blockSize = 1;

    int numBlocks = (N + blockSize - 1) / blockSize;

    // Ensure numBlocks doesn't exceed maxGridSize[0]
    if (numBlocks > props.maxGridSize[0]) {
        numBlocks = props.maxGridSize[0];
        // Potentially adjust N or warn the user if data won't fit/process
    }

    std::cout << "\nDynamically determined launch parameters for Device " << dev << ":" << std::endl;
    std::cout << "   blockSize: " << blockSize << std::endl;
    std::cout << "   numBlocks: " << numBlocks << std::endl;

    // ... then launch your kernel with these dynamic values:
    // vectorAdd<<<numBlocks, blockSize>>>(d_A, d_B, d_C, N);
``` 
