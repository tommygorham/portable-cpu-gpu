cmake_minimum_required(VERSION 3.18)
project(portable-cpu-gpu)

find_package(Kokkos REQUIRED)

if(Kokkos_ENABLE_OPENMP)
  find_package(OpenMP REQUIRED)
endif()

if(Kokkos_ENABLE_CUDA)
  find_package(CUDA REQUIRED)
endif()

if(Kokkos_ENABLE_MPI)
  find_package(MPI REQUIRED) 
  include_directories(${MPI_INCLUDE_PATH})
endif() 

add_executable(kokkos_mvdot MatrixVectorDot/kokkos_mvdot.cpp )
target_link_libraries(kokkos_mvdot $<$<BOOL:${Kokkos_ENABLE_CUDA}>:${CUDA_LIBRARIES}> Kokkos::kokkos)

# Hybrid MPI + GPUs 
if(Kokkos_ENABLE_MPI AND Kokkos_ENABLE_CUDA) 
  add_executable(kokkos_mpi_cuda_mvdot.cu kokkos_mpi_cuda_mvdot.cpp)
  target_link_libraries(kokkos_mpi_cuda_mvdot.cu ${MPI_LIBRARIES} ${CUDA_LIBRARIES} Kokkos::kokkos)
endif() 

add_executable(kokkos_gol GOL/kokkos_gol.cpp )
target_link_libraries(kokkos_gol $<$<BOOL:${Kokkos_ENABLE_CUDA}>:${CUDA_LIBRARIES}> Kokkos::kokkos)

add_executable(kokkos_pi Pi/kokkos_pi.cpp )
target_link_libraries(kokkos_pi $<$<BOOL:${Kokkos_ENABLE_CUDA}>:${CUDA_LIBRARIES}> Kokkos::kokkos)

add_executable(kokkos_sort Sort/kokkos_sort.cpp )
target_link_libraries(kokkos_sort $<$<BOOL:${Kokkos_ENABLE_CUDA}>:${CUDA_LIBRARIES}> Kokkos::kokkos)
