# Function to create and link executables with Kokkos, CUDA, and MPI support
function(create_kokkos_target target_name source_files)
  add_executable(${target_name} ${source_files})
  if(Kokkos_ENABLE_MPI)
    include_directories(${MPI_INCLUDE_PATH})
  endif()
  target_link_libraries(${target_name}
    Kokkos::kokkos
    $<$<BOOL:${Kokkos_ENABLE_CUDA}>:${CUDA_LIBRARIES}>
    $<$<BOOL:${Kokkos_ENABLE_MPI}>:${MPI_LIBRARIES}>)
endfunction()
