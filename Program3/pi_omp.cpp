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
#include <omp.h> // OpenMP runtime routines  

// accuracy of estimate vs actual pi 
void calcAccuracy(double pi, double est){
    double temp =  std::abs(pi-est);  
    double acc  = (temp/pi) * 100; 
    cout << "\npercent err: " << fixed << acc << endl;  
} 

static int N = 1e8; // problem size 

int main(int argc, char* argv[])
{
    // input
    double est, sum = 0.0; 
    const double pi =  3.141592653589793; // for reference    
    double step = 1.0/(double) N; 
     
    double start = omp_get_wtime(); // begin timer 
    #pragma omp parallel // begin parallel section 
    { 
        #pragma omp for reduction(+:sum)
        for(int i=0; i < N; ++i) { 
           sum += 4.0/(1.0+((i+0.5)*step) * ((i+0.5)*step)) ;
        } 
    }    
    est = step*sum; 
    // report time taken to compute 
    const double time = (omp_get_wtime() - start);    
    cout << "\npi_omp took " << fixed << time; 
    cout << "\nseconds to compute"; 
    
    // formatting 
    #define COUT(x) cout << "\n" << setw(4) << setprecision(15) << #x << x << endl;  
    cout << std::left;       
       
    // output 
    COUT(est);
    COUT(pi); 
    calcAccuracy(pi, est); 
        
    return 0; 
} 
