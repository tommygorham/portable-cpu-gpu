// Program Description: This Program Computes the Matrix-Vector Dot Product using Kokkos where 
// Equation: y=Ax or y=Bx (comparing 1D-2D storage) 
// A = modified from serial.cpp to be a Kokkos::View that is a 2D matrix of M rows, N columns 
// B = modified from serial.cpp to be a Kokkos::View that only has 1 run-time dimension, though represents a matrix size = (M*N) 
// x = modified from serial.cpp to be a Kokkos::View representing a 1D array/vector of size N
// y = modified from serial.cpp to be a Kokkos::View that will store our solution size =  M 
// Purpose: To demonstrate how to use transfer code to Kokkos for performance portability, 
// To Compile for and test with different backends enabled (Serial, OpenMP, Cuda, All 3, etc..)  
// Author: Thomas Gorham
// University of Tennessee at Chattanooga, Simcenter
// Date: 4/25/2022

#include <chrono> /* A C++ STD library to measure execution of matrix vector computation. Note: OpenMP can be 
                   * used to do this instead, as we did in serial2.cpp but only always compiling for openmp to
                   * obtain on-node parallelism. chrono::high_resolution_clock is used here to maximize 
                   * portability for instances where OpenMp is not compiled for.  */ 
#include <iostream> /* standard I/O in C++ */ 
#include "Kokkos_Core.hpp" /* gives us Kokkos Core Library for performance portability and to abstract CPU/GPU targets */ 

// types for Kokkos View abstraction. views let us move data back and forth from cpu/gpu  
typedef Kokkos::View<double*>  LinearType; /* for vectors and matrices in disguise (evil laugh) */ 
typedef Kokkos::View<double**> MatrixType;
typedef Kokkos::View<double*, Kokkos::LayoutRight> OptGPUType; /*to compare row major vs col major */ 

// ###  FUNCTION PROTOTYPES ###
/* Function 1: Modified from serial.cpp to print a two-dimensional Kokkos::view, as opposed to matrix 
   Here, we can use viewname.extent() for rows/cols. Function definition near end of file */  
void printMatrix(MatrixType::HostMirror matrix); 

// Function 2: Modified for printing contiguous 1D Kokkos::View data as a 2D matrix 
void print1DTo2D(LinearType::HostMirror data, const unsigned int rows, const unsigned int cols); 

// Function 3: Used to print a 1D Kokkos::View that resembles a 1D vector, using view.extent() 
void printVector(LinearType::HostMirror vec); 

/* Function 4: Used to Fill a 2D MatrixType view, just pass by value, this should be contiguous blocks if
   * execution space is CPU-based */ 
void fill2DMatrix(MatrixType::HostMirror data, const unsigned int M, const unsigned int N); 

/* Function 5: Used to test the execution time of multiplying a data at(i*N+j)*x(N) = y(M)  */  
void performDotProduct(LinearType data, LinearType x, LinearType y, unsigned int rows, unsigned int cols); 

// Function 6: testing to see what happens if i explicitly write col major storage in 
void performOptimizedKernelDot(OptGPUType data, LinearType x, LinearType y, unsigned int rows, unsigned int cols);

/* program entry point */ 
int main(int argc, char** argv) 
{
  /*NOTE: the only thing that should ever be above this is mpi init on distributed archs*/ 
  Kokkos::initialize(argc, argv); /* initialize kokkos core */ 
  { 
       // for iterating, never negative  
       unsigned int i, j;    
       // size of matrix and vector for y=Ax or y=Bx
       const unsigned int M = 1000; 
       const unsigned int N = 1000; 
    
       // TO DO: experiment with using run time dims via  Kokkos::View<double[M][N]>
       LinearType y("y", M); /* same vector as serial, just using Kokkos abstraction */ 
       LinearType x("x", N); /* same vector as serial, just using Kokkos abstraction */ 
       LinearType B("B", M*N); /* ensuring row major contiguous just as we did in serial.cpp with malloc*/ 
       /* MatrixType A("A", M, N); conventional 2d matrix using kokkos view abstraction for performance comparison testing later */ 
       /* adding special GPU optimizations*/ 
       OptGPUType C("C", M*N); /* This was for testing hard set execution policy, learned it is not needed*/  
        /* Not necessary, but printing our environment we compiled for. 
         * beginning with execution space, the value will trinkle down in this order depended on what was compiled for
         *  1. GPU, 2. OPENMP,3.Serial
         *  essentially host execution space is set after execution space chooses the highest available, meaning if execution
         *  space is CUDA, host exe is OpenMP (assuming we compiled for both) 
         */ 

       std::cout << "\nDefault execution space: " << typeid(Kokkos::DefaultExecutionSpace).name() << "\n" << std::endl; 
	   std::cout << "\nDefault Host execution space: " << typeid(Kokkos::DefaultHostExecutionSpace).name() << "\n" << std::endl;    
        
       /* For maximum portability, if we plan to potentially compile for the GPU (but not always), use Kokkos::create_mirror_view
        * instead of Kokkos::create_mirror. Only use create_mirror if we know we will only use this program to make use of GPU 
        * accelorators. The reason create_mirror_view is more portable is that it only creates a new view if the data within the 
        * original view is not already accessible within our hostspace
        */
        
       LinearType::HostMirror h_y = Kokkos::create_mirror_view( y );
       LinearType::HostMirror h_x = Kokkos::create_mirror_view( x );
       LinearType::HostMirror h_B = Kokkos::create_mirror_view( B );
       //MatrixType::HostMirror h_A = Kokkos::create_mirror_view( A );
       OptGPUType::HostMirror h_C = Kokkos::create_mirror_view( C ); //not needed, kokkos optimizes automatically via
      // compiler
 
       // Init LinearType vector y on host (needed for result computation checking) */ 
       for (i = 0; i < M; ++i) { h_y(i) = 1;}
      
       // Init LinearType vector x on host 
       for (i = 0; i < N; ++i) { h_x(i) = 1; }
        
       // Init LinearType 1D matrix B on host  
       for (i = 0; i < M; ++i)
       {
           for(j = 0; j<N; ++j)
           {
               h_B(i*N+j) = 1; /* row major */ 
           }
       }
      
      // Init OptGPUType Col Major  
       for (i = 0; i < M; ++i)
       {
           for(j = 0; j<N; ++j)
           {
               h_C(i+M*j) = 1;  //col major 
           }
       }
       /*can init views with functions too, just pass by value */ 
      // fill2DMatrix(h_A, M, N);  /*Init 2D Matrix A on host, for now just using B*/   

      /* Kokkos::deep_copy is used for copying the data in the view to another view in a different memory space 
       * Aka: CPU -> GPU. In other words, Kokkos::deep_copy is necessary if offloading work to GPU. In order to
       * maximize application portability for modern architectures, it's best to have this in the code so that
       * compiling for GPUs is no issue. 
       * NOTE: this is a no-op if Kokkos::memoryspace is in Kokkos::Hostspace */ 
       Kokkos::deep_copy(y, h_y); 
       Kokkos::deep_copy(x, h_x); 
       Kokkos::deep_copy(B, h_B); 
       Kokkos::deep_copy(C, h_C); 
             
        //moving this here from serial and increased 10 to 20 for more correctness testing
       if(N < 20 && M < 20)
       {
         printVector(h_x); 
         print1DTo2D(h_B, M, N);
         double result = 0; 
         performDotProduct(B, x, y, M, N); 
         Kokkos::deep_copy(h_y, y); 
         printVector(h_y); 
         result = (h_y.extent(0) * N);    /*check sol */
         std::cout << "Computed result of " << M << " * " << N << " = "  << result << std::endl; 
        }
       /*
        * next we are parellizing over the number of rows
        * @temp: the sum of *this* row 
        * Note: Apparently need a lot of rows to saturate NVIDIA Volta GPUs
        * 
        */ 
       else
       {
         std::cout << "\n\nTest1: Kokkos::ParallelReduce, Row Major Storage\n\n"; 
         double result = 0; 
         auto start = std::chrono::high_resolution_clock::now(); 
         performDotProduct(B, x, y, M, N); 
         auto end = std::chrono::high_resolution_clock::now();
         // Kokkos::fence(); 
         //Kokkos::deep_copy(h_y, y); 
         //printVector(h_y); 
         result = (h_y.extent(0) * N);    /*check sol */
         std::chrono::duration<double, std::milli> mstime = end - start; 
         std::cout << "\nDot Product Took: " << mstime.count() << " milliseconds"; 
         std::chrono::duration<double, std::micro> microtime = end - start;
         std::cout << "  or, " << microtime.count() << " microseconds" << std::endl;
         std::cout << "Computed result of " << M << " * " << N << " = "  << result << std::endl;
         Kokkos::fence(); 
         Kokkos::deep_copy(y ,h_y); /*copy ones back to device view for next test*/ 
        //For(i = 0; i < N; ++i) {y(i)=1;} 

		// testing 1d col major hard coded for kernel 
         std::cout << "\n\nTest2: Kokkos::ParallelReduce, Col Major Storage\n\n";
         double result2 = 0; 
         auto start2 = std::chrono::high_resolution_clock::now(); 
         performOptimizedKernelDot(C, x, y, M, N); 
         auto end2 = std::chrono::high_resolution_clock::now();
         //Kokkos::fence();
         Kokkos::deep_copy(h_y, y); 
         // printVector(h_y); 
         result2 = (h_y.extent(0) * N);    /*check sol */
         std::chrono::duration<double, std::milli> mstime2 = end2 - start2; 
         std::cout << "\nDot Product Took: " << mstime2.count() << " milliseconds"; 
         std::chrono::duration<double, std::micro> microtime2 = end2 - start2;
         std::cout << "  or, " << microtime2.count() << " microseconds" << std::endl;
         std::cout << "Computed result of " << M << " * " << N << " = "  << result2 << std::endl;
       }   //  Kokkos::fence(); 
  }   // close kokkos scope
  Kokkos::finalize();  /*use ths to free memory */ 
  return 0; 

} /*end main*/ 

//############################# FUNCTION DEFINITIONS #############################

//Function 1 Definition: Print 2D Kokkos::View on host using the views extent
void printMatrix(MatrixType::HostMirror matrix) 
{
    std::cout << "\n2D Matrix Name: " << matrix.label() << std::endl;
    for (unsigned int i=0; i<matrix.extent(0); ++i){
        for (unsigned int j=0; j<matrix.extent(1); ++j){
        std::cout<< matrix(i,j) << " ";
        }
      std::cout << "\n";
    }
}

// Function 2 Definition: Print 1D data in 2D matrix format using row major on host
void print1DTo2D(LinearType::HostMirror data, const unsigned int rows, const unsigned int cols)
{
    std::cout << "\nMatrix Name: " << data.label() << std::endl;/*label() to get view identity*/ 
    for (unsigned int i=0; i<rows; ++i){
        for (unsigned int j=0; j<cols; ++j){
        std::cout<< data[i*cols+j] << " "; /*Row major*/ 
        }
      std::cout << "\n";
    }
}
// Function 3 Definition: Print 1D Kokkos::View representing a vector in 1D using extent 
void printVector(LinearType::HostMirror vec)
{
    std::cout << "\nVector Name: " << vec.label() << std::endl; /*label() to get view identity*/ 
    for (unsigned int i=0; i<vec.extent(0); ++i){
        std::cout<<  vec(i) << " ";
    }
    std::cout << "\n";
}

/* New Function 4: Function to fill A and to demonstrate how views can easily be passed by value */ 
void fill2DMatrix(MatrixType::HostMirror data, const unsigned int M, const unsigned int N){
       /*fill */ 
       for (unsigned int i=0; i < M; ++i)
       {
           for(unsigned int j=0; j<N; ++j)
           {
              data(i,j)=1; /* row major */ 
           }
       }
}

// Function 5: Compute Dot Product to compare with serial implementation
// changed to void incase we compile for gpu
void performDotProduct(LinearType data, LinearType  x, LinearType  y, unsigned int rows,unsigned int cols)
 { 
   int result = 0; 
   Kokkos::parallel_reduce(
        "yAx", rows,
        KOKKOS_LAMBDA(unsigned int i,  int &update) { // compiler auto-generates a functor
          double this_row_sum = 0;
          for (unsigned int j = 0; j < cols; ++j) {
            this_row_sum += data(i*cols+j) * x(j);
          }
           update = this_row_sum;
           y(i) = update;
           //update += y(i) * this_row_sum;  
        },
        result);
}

// Function 6 Definition: 1D col major memory access test
void performOptimizedKernelDot(OptGPUType data, LinearType x, LinearType y, unsigned int rows, unsigned int cols)
{
   int result = 0; 
   Kokkos::parallel_reduce(
        "yAx", rows,
        KOKKOS_LAMBDA(unsigned int i,  int &update) { // compiler auto-generates a functor
          double this_row_sum = 0;
          for (unsigned int j = 0; j < cols; ++j) {
            //this_row_sum += data(j*rows+i) * x(j);/*col major*/ 
             this_row_sum += data(j*rows+i) * x(j);/*col major*/ 
          }
           update = this_row_sum;
           y(i) = update;
           //update += y(i) * this_row_sum;  
        },
        result);
}
