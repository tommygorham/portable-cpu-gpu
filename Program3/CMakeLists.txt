cmake_minimum_required(VERSION 3.10)
project(montecarlo)

find_package(Kokkos REQUIRED)
#find_package (OpenMP REQUIRED)
#find_package(CUDA REQUIRED)

#set(CMAKE_CXX_FLAGS "-Wall ")

add_executable(run_mc_serial mc_pi_kokkos.cpp)
target_link_libraries(run_mc_serial Kokkos::kokkos)

###just to separate cpu parallelism from serial execution
add_executable(run_mc_par mc_pi_kokkos.cpp)
target_link_libraries(run_mc_par Kokkos::kokkos)

###if using cuda/gpu
#add_executable(run_mc_gpu mc_pi_kokkos.cpp )
#target_link_libraries(run_mc_gpu ${CUDA_LIBRARIES} Kokkos::kokkos)
