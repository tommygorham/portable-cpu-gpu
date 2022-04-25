// Program Description: This Program Computes the Matrix-Vector Dot Product using Kokkos where Equation: y=Ax. 
// This version uses Hierarchical parallelism via Kokkos::TeamPolicy such that total work = #teams * teamsize 
// Purpose: To exploit more parallelism and performance portability on diverse CPU and GPU targets  
// Author: Thomas Gorham
// University of Tennessee at Chattanooga, Simcenter
// Date: 4/25/2022

#include <chrono> 
#include <iostream> 
#include "Kokkos_Core.hpp" /* gives us Kokkos Core Library for performance portability and to abstract CPU/GPU targets */ 

/* program entry point */ 
int main(int argc, char** argv) 
{
  Kokkos::initialize(argc, argv); /* initialize kokkos core */ 
  { 
       /* global vars still the same*/ 
       unsigned int i, j;    
       const unsigned int M = 1000; /*rows */ 
       const unsigned int N = 1000; /* cols */ 
       
       // Custom types of data structures (Views) 
       // Here I'm using traditional matrix storage now since Kokkos already optimizes contiguous chunks
       // aka no need for i*cols+j 
       typedef Kokkos::View<double**, Kokkos::LayoutRight> Matrix; 
       typedef Kokkos::View<double*>  Vector; 
       typedef Kokkos::TeamPolicy<>               team_policy; /*instead of execution policy*/ 
       typedef Kokkos::TeamPolicy<>::member_type  member_type;
       
       // creating A, x, y     
       Matrix A("A", M, N); 
       Vector x("x", N); 
       Vector y("y", M); 

       // host views
       Matrix::HostMirror h_A = Kokkos::create_mirror_view(A);  /*remember, this is either a reference or a CPU Copy that can potentially help us*/ 
       Vector::HostMirror h_x = Kokkos::create_mirror_view(x);
       Vector::HostMirror h_y = Kokkos::create_mirror_view(y);

       // Initializing A, x, y with data 
       for (i = 0; i < M; ++i) { h_y(i) = 1;}
       for (i = 0; i < N; ++i) { h_x(i) = 1; }
       for (i = 0; i < M; ++i) {
           for(j = 0; j<N; ++j){
               h_A(i,j) = 1; /* 2D memory access */ 
           }
       }
      
      // Remember, this is a no-op if Kokkos::memoryspace is in Kokkos::Hostspace */ 
       Kokkos::deep_copy(y, h_y); 
       Kokkos::deep_copy(x, h_x); 
       Kokkos::deep_copy(A, h_A); 
       
       /* Obtain more portability and parallelism: specify team size and # of teams
        * a 'thread team' is guaranteed to execute concurently, and can synchronize
        * where execution policy says total work = N, team policy says total work 
        * is equivalent to #threadteams * teamsize, where teamsize is the number of
        * threads assigned to each of the work items. Here, I'm launching 1 team per
        * row via team_policy(M),  Kokkos::AUTO can be used if you don't know what
        * team size you need  */ 
       double result = 0;
       auto start = std::chrono::high_resolution_clock::now(); 
       Kokkos::parallel_reduce( "yAx", team_policy( M, Kokkos::AUTO), KOKKOS_LAMBDA ( const member_type &teamMember, double &update ) 
       {
         int i = teamMember.league_rank(); /*league_rank == which team am i on*/ 
         double this_rows_sum = 0;

         /*nested reduction, split the range of N over the threads in the team */ 
         Kokkos::parallel_reduce(Kokkos::TeamThreadRange( teamMember, N ), [=] ( const int j, double &innerUpdate )
         {
             innerUpdate += A(i,j) * x(j); 
         }, this_rows_sum);
        
        // since we are using shared memory, every thread is storing the reduction value
        if (teamMember.team_rank() == 0) { update+= y(i) * this_rows_sum;}
       }, result );
       
       auto end = std::chrono::high_resolution_clock::now(); 
       std::chrono::duration<double, std::milli> mstime = end - start; 

       std::cout << "Computed result of " << M << " * " << N << " = "  << result; 
       std::cout << "\n\nMultiplication took: " << mstime.count() << " milliseconds\n"; 
       //Teams: " << teamMember.league_size();        

  }   // close kokkos scope
  Kokkos::finalize();  /*use ths to free memory */ 
  return 0; 

} /*end main*/ 

//############################# FUNCTION DEFINITIONS #############################

