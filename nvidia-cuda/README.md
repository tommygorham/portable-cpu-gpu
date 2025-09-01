# Cuda Programs 
```
# Tested with cuda 11.2, checks that everything is installed correctly and ready to go 
nvcc hello-cuda.cu -o hello-cuda

# Most impt 
cuda-device-query.cu 

```

## Comparison to Hip/Rocm
* Still more convenient -- 
When nvcc compiles a .cu file, it implicitly includes cuda\_runtime.h (or a more fundamental header like
cuda\_runtime\_api.h) behind the scenes. This is a convenience feature of nvcc and why it doesn't have to 
include it like hello-hip-rocm.cpp does

The printf() inside the helloWorldKernel()) is a device-side printf. It's not the standard C/C++ library
printf that runs on the CPU. CUDA provides its own version of printf for device code. 
