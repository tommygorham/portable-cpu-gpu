#include <sys/time.h>
#include <Kokkos_Core.hpp>

int main(int argc, char **argv)
{
  int N = 10000; /* global rows */ 
  int M = 10000; /* global cols */ 
 
  Kokkos::initialize( argc, argv ); //start scope of kokkos
  {

	// if we compile for the GPU
     #ifdef KOKKOS_ENABLE_CUDA
     #define MemSpace Kokkos::CudaSpace
     #endif
	// if we compile for the CPU
     #ifndef MemSpace
     #define MemSpace Kokkos::HostSpace
     #endif
	// to run code via how we store it
     using ExecSpace = MemSpace::execution_space; 
     using range_policy = Kokkos::RangePolicy<ExecSpace>; /*as opposed to mpi partitioning*/ 

    // make data structures 
     typedef Kokkos::View<double*, Kokkos::LayoutLeft, MemSpace>   ViewVectorType;
     typedef Kokkos::View<double**, Kokkos::LayoutLeft, MemSpace>  ViewMatrixType;
     ViewVectorType y( "y", N );
     ViewVectorType x( "x", M );
     ViewMatrixType A( "A", N, M );

     // Create host mirrors of device views.
     ViewVectorType::HostMirror h_y = Kokkos::create_mirror_view( y );
     ViewVectorType::HostMirror h_x = Kokkos::create_mirror_view( x );
     ViewMatrixType::HostMirror h_A = Kokkos::create_mirror_view( A );

     // Initialize y vector on host.
     for ( int i = 0; i < N; ++i ) {
      h_y( i ) = 1;
     }

    // Initialize x vector on host.
    for ( int i = 0; i < M; ++i ) {
      h_x( i ) = 1;
    }

    // Initialize A matrix on host.
    for ( int j = 0; j < N; ++j ) {
      for ( int i = 0; i < M; ++i ) {
        h_A( j, i ) = 1;
      }
    }

    /* for gpu */ 
    Kokkos::deep_copy( y, h_y );
    Kokkos::deep_copy( x, h_x );
    Kokkos::deep_copy( A, h_A );

    // time solution with Kokkos
    Kokkos::Timer timer;

	// matrix-vector multiply
    double result = 0;
    Kokkos::parallel_reduce( "yAx", range_policy( 0, N ), KOKKOS_LAMBDA ( int j, double &update ) {
      double temp2 = 0;

      for ( int i = 0; i < M; ++i ) {
        temp2 += A( j, i ) * x( i );
      }
      update += y( j ) * temp2;
    }, result );

    // Output result.
	std::cout << "result is " << result << std::endl; /* should be N*M */ 
    
    const double solution = (double) N * (double) M;
	if ( result != solution ) {
       std::cout << "  Error: result != solution";
	}

    // Calculate time.
    double time = timer.seconds();
    std::cout << "time: " << time << std::endl; 
    }
    Kokkos::finalize();

  return 0;
}