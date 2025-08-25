cmake_minimum_required(VERSION 3.21)
project(portable-cpu-gpu)

# Find Library Dependencies 
find_package(Kokkos REQUIRED)
if(Kokkos_ENABLE_OPENMP)
  find_package(OpenMP REQUIRED)
endif()
if(Kokkos_ENABLE_CUDA)
  find_package(CUDA REQUIRED)
endif()
if(Kokkos_ENABLE_MPI)
  find_package(MPI REQUIRED) 
endif() 

# Create Executable targets
include(${CMAKE_SOURCE_DIR}/KokkosTargetNames.cmake)  # Defines a list of ${TARGET_NAME}                     
include(${CMAKE_SOURCE_DIR}/CreateKokkosTarget.cmake) # Defines a function create_kokkos_target()
foreach(TARGET_NAME IN LISTS KOKKOS_TARGETS)
  create_kokkos_target(${TARGET_NAME} "${TARGET_NAME}.cpp")
endforeach()
