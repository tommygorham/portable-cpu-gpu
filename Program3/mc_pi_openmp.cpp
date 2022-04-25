// Program: An openmp-only implementation that uses a Monte Carlo algorithm to compute PI. 
// Purpose: To demonstrate the ease of porting embarassingly parallel problems to Kokkos and
// to compare to Kokkos::OpenMP backend  
// Note: Thread-safe version! Seeded for proovably non-overlapping sequences for maximum statistical quality.
// TO DO: Update with leap-from method so that the pseudo-random sequence generated is the same, regardless of the number of threads. 
// Special Thanks: Tim Mattson of Intel. He has great videos on this. 
// All I have done is added some comments and converted to c++ to use this openmp version as a comparison 
#include "omp.h"
#include <iostream> 


static unsigned long long MULTIPLIER  = 764261123;
static unsigned long long PMOD        = 2147483647;
static unsigned long long mult_n;
double random_low, random_hi;

#define MAX_THREADS 128
static unsigned long long pseed[MAX_THREADS][4]; //[4] to padd to cache line
                                                 //size to avoid false sharing
unsigned long long random_last = 0;
#pragma omp threadprivate(random_last)

double drandom()
{
    unsigned long long random_next;
    double ret_val;

// 
// compute an integer random number from zero to mod
//
    random_next = (unsigned long long)((mult_n  * random_last)% PMOD);
    random_last = random_next;

//
// shift into preset range
//
    ret_val = ((double)random_next/(double)PMOD)*(random_hi-random_low)+random_low;
    return ret_val;
}

//
// set the seed, the multiplier and the range
//
void seed(double low_in, double hi_in)
{
   int i, id, nthreads;
   unsigned long long iseed;
   id = omp_get_thread_num();

   #pragma omp single
   {
      if(low_in < hi_in)
      { 
         random_low = low_in;
         random_hi  = hi_in;
      }
      else
      {
         random_low = hi_in;
         random_hi  = low_in;
      }
  
//
// The Leapfrog method ... adjust the multiplier so you stride through
// the sequence by increments of "nthreads" and adust seeds so each 
// thread starts with the right offset
//

      nthreads = omp_get_num_threads();
      iseed = PMOD/MULTIPLIER;     // just pick a reasonable seed
      pseed[0][0] = iseed;
      mult_n = MULTIPLIER;
      for (i = 1; i < nthreads; ++i)
      {
	      iseed = (unsigned long long)((MULTIPLIER * iseed) % PMOD);
	      pseed[i][0] = iseed;
	      mult_n = (mult_n * MULTIPLIER) % PMOD;
      }
   }
   random_last = (unsigned long long) pseed[id][0];
}

static long num_trials = 1000000;

int main () 
{
   long i;  long Ncirc = 0;
   double pi, x, y, test, time; 
   double r = 1.0;   /* // radius of circle. Side of squrare is 2*r*/ 
   
   
   time = omp_get_wtime();
   #pragma omp parallel
   {

      #pragma omp single
          printf(" %d threads ",omp_get_num_threads());

      seed(-r, r);  
      #pragma omp for reduction(+:Ncirc) private(x,y,test)
      for(i=0;i<num_trials; i++)
      {
         x = drandom(); 
         y = drandom();

         test = x*x + y*y;

         if (test <= r*r) Ncirc++;
       }
    }

    pi = 4.0 * ((double)Ncirc/(double)num_trials);

    std::cout << "\nTime: " << omp_get_wtime()-time;
    std::cout << "\nTrials: " << num_trials << "  Pi: " << pi;  
    return 0;
}

   
