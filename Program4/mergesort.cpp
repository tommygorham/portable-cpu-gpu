#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>
#include <impl/Kokkos_Timer.hpp>
#include <cstdio>
#include <cstdlib>

using Device = Kokkos::DefaultExecutionSpace;
using Host   = Kokkos::HostSpace::execution_space;

using team_policy = Kokkos::TeamPolicy<Device>;
using team_member = team_policy::member_type;

static const int TEAM_SIZE = 16;


int main(int narg, char* args[]) {
  Kokkos::initialize(narg, args);

  {
    int chunk_size = 1024; /*threads per block reset to 32*/ 
    int nchunks    = 100000;  // blocks per grid reset to 8 */ 
    Kokkos::DualView<int*> data("data", nchunks * chunk_size + 1);

	long size;  

	// fill host view with something like srand(1231093);
	// maybe loop to size for testing first
    for (int i = 0; i < (int)data.extent(0); i++) {
      data.h_view(i) = rand() % TEAM_SIZE;
    }
    data.modify<Host>();
    data.sync<Device>();
	
	// deep copy removed
	
	//call kernel with a kokkos parallel for, cuda version is 
	for (int width = 2; width < (size << 1); width <<= 1) {
    long slices = size / ((nThreads) * width) + 1;
	// Kokkos: mergesort(data, size, threadsPerBlock, blocksPerGrid);
   // CUDA:  gpu_mergesort<<<blocksPerGrid, threadsPerBlock>>>
    //    (A, B, size, width, slices, D_threads, D_blocks);

    // Switch the input / output arrays
   // A = A == D_data ? D_swp : D_data;
    // B = B == D_data ? D_swp : D_data;
}

 
    // threads/team is automatically limited to maximum supported by the device.
   //  int team_size = TEAM_SIZE;
    // if (team_size > Device::execution_space::concurrency())
      team_size = Device::execution_space::concurrency();
   // Kokkos::parallel_for(team_policy(nchunks, team_size),

  Kokkos::finalize();
}

// kernel 1 uses kernel 2 below as helper func 
__global__ void gpu_mergesort(long* source, long* dest, long size,
                              long width, long slices,
                              dim3* threads, dim3* blocks) {
    unsigned int idx = getIdx(threads, blocks);
    long start = width*idx*slices,
         middle,
         end;
 
    for (long slice = 0; slice < slices; slice++) { 
        if (start >= size)
            break;
 
        middle = min(start + (width >> 1), size);
        end = min(start + width, size);
        gpu_bottomUpMerge(source, dest, start, middle, end);
        start += width;
    }
}

//kernel 2 
__device__ void gpu_bottomUpMerge(long* source, long* dest,
                                  long start, long middle,
                                  long end) {
    long i = start;
    long j = middle;
    for (long k = start; k < end; k++) {
        if (i < middle && (j >= end || source[i] < source[j])) {
            dest[k] = source[i];
            i++;
        } else {
            dest[k] = source[j];
            j++;
        }
    }
}