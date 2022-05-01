cmake_minimum_required(VERSION 3.10)
project(Program4)

find_package(Kokkos REQUIRED)
find_package (OpenMP REQUIRED)
#find_package(CUDA REQUIRED)

#set(CMAKE_CXX_FLAGS "-Wall ")

#add_executable(run_ms ms.cpp)
#target_link_libraries(run_ms Kokkos::kokkos)

#add_executable(run_ms_test testing.cpp)
#target_link_libraries(run_ms_test Kokkos::kokkos)

add_executable(run_finaltest finaltest.cpp)
target_link_libraries(run_finaltest Kokkos::kokkos)


#add_executable(run_ms_gpu.cu ms.cpp )
#target_link_libraries(run_ms_gpu.cu ${CUDA_LIBRARIES} Kokkos::kokkos)
