// Program Description: This Program Computes the Matrix-Vector Dot Product sequentially where 
// Equation: y=Ax 
// A = a contigous matrix of M rows, N columns 
// x = a vector of size N
// y = the solution vector of size M 
// Purpose: To show how this is computed, and to compare the execution time  to Kokkos::Serial 
// Author: Thomas Gorham
// University of Tennessee at Chattanooga, Simcenter
// Date: 4/25/2022

#include <iostream> 
#include <chrono> /* for measuring execution time */ 

// FUNCTION PROTOTYPES
// Function 1: Used to print a row major matrix  
void printMatrix(double*  matrix, const unsigned int rows, const unsigned int cols); 

// Function 2: Used to print a vector 
void printVector(double* vec, const unsigned int size); 

// Function 3: Used to print the result of multiplying A(M,N)*x(N) = y(M)  
void printDotProduct(double* A, double* x, double* y, const unsigned int rows, const unsigned int cols); 

/* Function 4: Used to test the execution time of multiplying A(M,N)*x(N) = y(M)  */  
int  performDotProduct(double* A, double* x, double* y, const unsigned int rows, const unsigned int cols); 

// NOTE: if we were optimizing serial code, I would convert the above two functions into a functor
// This program is for simplicity regarding our Kokkos::serial execution time comparisons

/* Program entry point */ 
int main(int argc, char *argv[])
{
    unsigned int i, j; /* for iterating */ 
    
    // size of matrix and vector for y=Ax
    const unsigned int M = 1000; // rows
    const unsigned int N = 1000; // cols

    // store size of matrix on heap, using static_cast as it is faster than dynamic_cast
    /* using double to measure signed 64-bit double-precision, a bit more precise than float (double is 8 bytes) */ 
    auto y = static_cast<double*>(std::malloc(M * sizeof(double))); 
    auto x = static_cast<double*>(std::malloc(N * sizeof(double)));
    auto A = static_cast<double*>(std::malloc(M * N * sizeof(double)));
    
    // initialize vector x 
    for (i = 0; i < N; ++i ) { x[ i ] = 1; }
    
    // initialize matrix A 
    for (i = 0; i < M; ++i ) {
        for (j = 0; j < N; ++j) {
            A[ i * N + j ] = 1; //row major, treating 1D like 2D 
        }
    }
    /* FOR DEBUGGING AND VISIBILITY */ 
    if (N < 10 && M < 10) /*if size is small, print our data structures and the result of multiplication to terminal */  
    {
        std::cout<<"\nMatrix A: \n";
        printMatrix(A, M, N);
        std::cout<<"\nTimes vector x: \n";
        printVector(x, N);
        printDotProduct(A, x, y, M, N);
        std::cout<<"\nequals vector y: \n";
        printVector(y, M);
    }
     
    /* for larger sizes (M, N), only check solution, print 1 integer result which should = M*N, and measure performance */  
      else
      {
        /* if size is not small */  
       auto start = std::chrono::high_resolution_clock::now(); 
       int sum_dot_result = performDotProduct(A, x, y, M, N); 
       auto end   = std::chrono::high_resolution_clock::now(); 
       std::chrono::duration<double, std::milli> mstime = end - start; 
       std::cout << "\nDot Product Took: " << mstime.count() << " milliseconds"; 
       std::chrono::duration<double, std::micro> microtime = end - start;
       std::cout << "  or, " << microtime.count() << " microseconds" << std::endl;  
       /* use the method call below with caution depending on how large M,N is */ 
       // printVector(y,M); 
       std::cout << "Computed result of " << M << " * " << N << " = "  << sum_dot_result << std::endl; 
      }

// Release dynamic memory
        std::free(A);
        std::free(y);
        std::free(x);
        return 0;
} /*end main*/ 

//Function 1: Print row major matrix definition
void printMatrix(double*  matrix, const unsigned int rows, const unsigned int cols)

{
    std::cout << "\n";
    for (unsigned int i=0; i<rows; ++i){
        for (unsigned int j=0; j<cols; ++j){
        std::cout<< matrix[i*cols+j] << " ";
        }
      std::cout << "\n";
    }
}
// Function 2: Print vector definition 
void printVector(double* vec, const unsigned int size)
{
    std::cout << "\n";
    for (unsigned int i=0; i<size; ++i){
        std::cout<<  vec[i] << " ";
    }
    std::cout << "\n";
}

// Function 3: Print Dot Product operations for small sizes. This aids in learning and also ensures correctness
void printDotProduct(double* A, double* x, double* y, const unsigned int rows, const unsigned int cols)
{   
    int rowdot = 0; /*this variable will store the total dot product for each row*/  
    for (unsigned int i = 0; i < rows; ++i)
    {
        int temp_row_dot=0; /*this variable tracks the current dot product for each iteration of the nested loop*/ 
        for(unsigned int j = 0; j < cols; ++j)
        { 
            temp_row_dot += A[ i * cols + j ] * x[ j ]; 
			std::cout << "\n A[ i * cols + j ]: " << A[ i * cols + j ] << "  times x[ j ]: " << x[ j ] << "    = " <<  A[ i * cols + j ] * x[ j ] << " + prev result = " << temp_row_dot;
		}
		rowdot = temp_row_dot;
		y[i] = temp_row_dot;
		std::cout << "\nrow dot: " << rowdot;
    }
    std::cout << "\n";
}

// Function 4: Compute Dot Product and return result, which should = M*N
int performDotProduct(double* A, double* x, double* y, const unsigned int rows, const unsigned int cols)
{ 
    int result = 0; /*this variable will store the total dot product for each row*/  
    for (unsigned int i = 0; i < rows; ++i)
    {
        int temp_row_dot=0; /*this variable tracks the current dot product for each iteration of the nested loop*/ 
        for(unsigned int j = 0; j < cols; ++j)
        { 
            temp_row_dot += A[ i * cols + j ] * x[ j ]; 
		}
			y[i] = temp_row_dot;
        	result = y[i] * temp_row_dot;
    }
    return result; 
}

