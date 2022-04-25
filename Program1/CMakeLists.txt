cmake_minimum_required(VERSION 3.10)

project(matrix-vector_matrix-matrix)

find_package(Kokkos REQUIRED) #keep this always
find_package(OpenMP REQUIRED) #uncomment this and lines 16,17. Comment out the lines 12,13 and 20,21
find_package(CUDA REQUIRED)  #uncomment this and the lines 20-21. Comment out 16,17 and 12,13

set(CMAKE_CXX_FLAGS "-Wall ")

### For SERIAL CPU execution  
#add_executable(runSerialCPU kokkos_mvdot.cpp )
#target_link_libraries(runSerialCPU Kokkos::kokkos)

#For OPENMP on-node parallelism
#add_executable(runParallelCPU kokkos_mvdot.cpp )
#target_link_libraries(runkokkosParallelCPU Kokkos::kokkos)

###FOR GPU, SERIAL + GPU, OPENMP + GPU, OR ALL 3 
add_executable(runkokkosGPU.cu kokkos_mvdot.cpp )
target_link_libraries(runkokkosGPU.cu ${CUDA_LIBRARIES} Kokkos::kokkos)
