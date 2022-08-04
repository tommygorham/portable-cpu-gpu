// COMPUTE PI 
// eq., 4/1+x^2 from 0-1 
// SERIAL VERSION 
#include <iostream> 
using std::cout; 
using std::endl;
#include <iomanip> 
using std::setprecision; 
using std::setw; 
using std::fixed; 
#include <chrono> // need a timer 

// accuracy of estimate vs actual pi 
void calcAccuracy(double pi, double est){
    double temp = std::abs(pi - est); 
    double acc  = (temp/pi) * 100; 
    cout << "\npercent err: " << fixed << acc << endl; 
}

static int N = 1e8; // problem size 

int main(int argc, char* argv[])
{
    // input 
    double est, sum = 0.0; 
    const double pi =  3.141592653589793; // for reference  
    double step     = 1.0/(double) N; 

     // compute  
    auto start = std::chrono::high_resolution_clock::now(); 
    for(int i = 0; i < N; ++i) { 
        sum += 4.0/(1.0+((i+0.5)*step) * ((i+0.5)*step)) ;
    } 
    est = step*sum; 
    // report time taken 
    auto end  = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double, std::ratio<1>> serial_time = end-start;  
    cout << "\npi_serial took " << fixed << serial_time.count() << endl; 
    
    // formatting 
    #define COUT(x) cout << "\n" << setw(4) << setprecision(15) << #x << x << endl;  
    cout << std::left;   
    
    // output 
    COUT(est);
    COUT(pi);  
    calcAccuracy(pi, est); 
    
    return 0; 
} 
