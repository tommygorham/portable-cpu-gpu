HIP/ROCm Programs 
``` 
# Tested with rocm 6.4.0, checks that everything is installed correctly and ready to go 
hipcc hello-hip-rocm.cpp -o hello-hip-rocm
# Use with command: watch -n 1 rocm-smi 
hipcc hip-vec-add.cpp 
```

