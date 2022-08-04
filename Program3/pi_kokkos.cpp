// Compute PI 
// eq., 4/1+x^2 from 0-1
// OPENMP VERSION  
#include <iostream> 
using std::cout; 
using std::endl;
#include <iomanip> 
using std::setprecision; 
using std::setw; 
using std::fixed; 
#include "Kokkos_Core.hpp" // Kokkos environment  

// accuracy of estimate vs actual pi  
void calcAccuracy(double pi, double est) 
{ 
    double temp = std::abs(pi-est); 
    double acc = (temp/pi)*100; 
    cout << "\npercenterror: " << fixed << acc << endl; 
}

static int N = 1e8; // problem size 

int main(int argc, char* argv[])
{ 
    Kokkos::initialize(argc, argv); // init Kokkos environment  
    // input 
    double est, sum = 0.0; 
    const double pi =  3.141592653589793; // for reference   
    double step = 1.0/(double) N; 
    
    Kokkos::Timer timer; // begin timer
    // begin parallel section
    Kokkos::parallel_reduce("compute_pi", N, [=] (const int i, double& update){
           update += 4.0/(1.0+((i+0.5)*step) * ((i+0.5)*step)) ;
       },sum);
   est = step*sum; 
   // report time taken 
   const double t = timer.seconds(); 
   cout << "\npi_kokkos.cpp took " << fixed <<  t; 
   cout <<" \nseconds to compute\n"; 
  
    // formatting 
    #define COUT(x) cout << "\n" << setw(4) << setprecision(15) << #x << x << endl;  
    cout << std::left;       
       
    // output 
    COUT(est);
    COUT(pi); 
    calcAccuracy(pi, est); 

    Kokkos::finalize(); // close kokkos environment  

    return 0; 
} 
