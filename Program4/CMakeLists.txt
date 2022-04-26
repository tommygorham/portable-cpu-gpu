cmake_minimum_required(VERSION 3.10)
project(Program4)

find_package(Kokkos REQUIRED)
#find_package (OpenMP REQUIRED)
#find_package(CUDA REQUIRED)

#set(CMAKE_CXX_FLAGS "-Wall ")

add_executable(run_ms mergesort.cpp)
target_link_libraries(run_ms Kokkos::kokkos)

#add_executable(run_gpu_par_gol.cu kokkos_gol.cpp )
#target_link_libraries(run_gpu_par_gol.cu ${CUDA_LIBRARIES} Kokkos::kokkos)
