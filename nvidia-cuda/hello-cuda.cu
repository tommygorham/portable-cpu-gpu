#include <iostream>

__global__ void helloWorldKernel() {
    printf("Hello from GPU world!\n");
}

int main() {
    helloWorldKernel<<<1, 1>>>();
    cudaDeviceSynchronize(); // Wait for the kernel to finish
    std::cout << "Hello from CPU world!" << std::endl;
    return 0;
}
