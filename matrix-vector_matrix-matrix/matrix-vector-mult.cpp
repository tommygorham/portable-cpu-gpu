#include "Kokkos_Core.hpp"
#include <iostream>

int main(int argc, char **argv) {

  Kokkos::initialize(argc, argv); // start kokkos scope
  {
      
	// OPTIMIZATIONS 
	  //enable if cpu 
	  typedef Kokkos::Serial ExecSpace; 
	  typedef Kokkos::HostSpace MemSpace; 
	  typedef Kokkos::LayoutRight Layout; // store contiguous for cpu for higher performance 
	
	  //enable if gpu
	  //typedef Kokkos::Cuda ExecSpace; 
	  //typedef Kokkos::Hostspace MemSpace; 
	  //typedef Kokkos::LayoutLeft Layout; //store strided for GPU for higher performance
	  
	// BUILD DATA STRUCTURES 
	  
	  const int M = 5; /*global size*/
      //const int globalM = atoi(argv[1]); /*global rows*/   
      // std::cout << "M is " << M << std::endl; 
    
	  typedef Kokkos::View<double*,Layout,MemSpace> ViewVectorType; 
	  typedef Kokkos::View<double**,Layout,MemSpace> ViewMatrixType;
	  
	  ViewVectorType y("y", M); /*solution vector*/ 
	  ViewVectorType x("x", M); /*vector for A*x*/ 
	  ViewMatrixType A("A", M); /*matrix A for A*x */ 
	  
	 
	  // host views 
	  ViewVectorType::HostMirror h_y = Kokkos::create_mirror_view(y);
      ViewVectorType::HostMirror h_x = Kokkos::create_mirror_view(x);
      ViewMatrixType::HostMirror h_A = Kokkos::create_mirror_view(A);
	  
	  
	// FILL HOST VIEWS ON CPU
	
	// Initialize A matrix on host.
	  for (int i = 0; i < M; ++i) {
        for (int j = 0; j < M; ++j) {
          h_A(i, j) = 1;
        }
      }

    // Initialize x vector on host ( mpiranks in row comm)
      for (int i = 0; i < M; ++i) {
        h_x(i) = 1;
        }
      }
	  
	// DEEP COPY IF GPU ENABLED
      Kokkos::deep_copy(y, h_y);
      Kokkos::deep_copy(x, h_x);
      Kokkos::deep_copy(A, h_A);

	// MULTIPLY
	  double result = 0.0;
	  Kokkos::parallel_reduce(
        "yAx", M,
        KOKKOS_LAMBDA(int i,
                      double &update) { // compiler auto-generates a functor
          double temp2 = 0;
          for (int j = 0; j < M; ++j) {
            temp2 += A(i, j) * x(j);
          }
          update = temp2;
          y(i) = update;
          //  std::cout << " \nrow dot = " << update;
        },
        result);

*/

	
  } //close kokkos scope 
  
  Kokkos::finalize();

  return 0;
}