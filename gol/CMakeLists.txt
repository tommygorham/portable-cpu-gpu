cmake_minimum_required(VERSION 3.10)

project(gol)

find_package(Kokkos REQUIRED)
find_package(CUDA REQUIRED)

#set(CMAKE_CXX_FLAGS "-Wall ")

#add_executable(runKokkosGol kokkosGOL.cpp )
#target_link_libraries(runKokkosGol Kokkos::kokkos)

add_executable(runKokkosGol kokkosGOL.cpp )
target_link_libraries(ronKokkosGol ${CUDA_LIBRARIES} Kokkos::kokkos)
