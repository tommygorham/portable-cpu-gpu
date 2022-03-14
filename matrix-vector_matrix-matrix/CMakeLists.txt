cmake_minimum_required(VERSION 3.10)

project(matrix-vector_matrix-matrix)

find_package(Kokkos REQUIRED)
#find_package(CUDA REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall ")

add_executable(run_mv mvmult.cpp )
target_link_libraries(run_mv Kokkos::kokkos)

#target_link_libraries(new.cu ${CUDA_LIBRARIES} Kokkos::kokkos)
