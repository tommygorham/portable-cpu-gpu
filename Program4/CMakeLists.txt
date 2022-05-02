cmake_minimum_required(VERSION 3.10)
project(Program4)

find_package(Kokkos REQUIRED)
#find_package (OpenMP REQUIRED)
#find_package(CUDA REQUIRED)

#set(CMAKE_CXX_FLAGS "-Wall ")

add_executable(run_sort kokkos-sort.cpp)
target_link_libraries(run_sort Kokkos::kokkos)

#for cpu parallelism openmp 
#add_executable(run_sort_par kokkos-sort.cpp)
#target_link_libraries(run_sort Kokkos::kokkos)

#for gpu parallelism and/or gpu 
#add_executable(run_sort_gpu.cu kokkos-sort.cpp )
#target_link_libraries(run_sort_gpu.cu ${CUDA_LIBRARIES} Kokkos::kokkos)
