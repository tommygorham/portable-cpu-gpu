# Define a cmake list of the executable targets we will build, from the C++ name
# To add a new target you only need to add the name of the C++ file to this list
set(KOKKOS_TARGETS
    kokkos_mpi_cuda_mvdot
    kokkos_gol
    kokkos_pi
    kokkos_sort
)
