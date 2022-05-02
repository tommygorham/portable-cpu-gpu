/* Program Description:  
                  This program uses a Monte Carlo algorithm to compute PI as an
                  example of how random number generators are used to solve problems.
                  Kokkos provides two different random number generators with a 64-bit
                  and a 1024-bit state.              
   Purpose: 
                  To Compare CPU/GPU runtimes and test against pure openmp/cuda
                  implementations. 
*/ 

#include <Kokkos_Core.hpp>
#include <Kokkos_Random.hpp>
#include <impl/Kokkos_Timer.hpp>
#include <cstdlib>


// A Functor for generating uint64_t random numbers templated on the GeneratorPool type
template<class RandPool>
struct GenRandom {

   // The GeneratorPool
   RandPool rand_pool;

   typedef double Scalar;
   typedef typename RandPool::generator_type gen_type;
           
   Scalar rad;   //target radius and box size
   long dart_groups; // Reuse the generator for drawing random #s this many times

   KOKKOS_INLINE_FUNCTION
   void operator() (long i, long& lsum) const {
     // Get a random number state from the pool for the active thread
     gen_type rgen = rand_pool.get_state();

     // Draw samples numbers from the pool as urand64 between 0 and rand_pool.MAX_URAND64
     for ( long it = 0; it < dart_groups; ++it ) {
       Scalar x = Kokkos::rand<gen_type, Scalar>::draw(rgen);
       Scalar y = Kokkos::rand<gen_type, Scalar>::draw(rgen);

     Scalar dSq = (x*x + y*y);

     if (dSq <= rad*rad) { ++lsum; } // comparing to rad^2 - am I in the circle inscribed in square?
   }

   // Give the state back, which will allow another thread to aquire it
   rand_pool.free_state(rgen);
 }  

 // Constructor, Initialize all members
 GenRandom(RandPool rand_pool_, Scalar rad_, long dart_groups_)
   : rand_pool(rand_pool_), rad(rad_), dart_groups(dart_groups_) {}

}; //end GenRandom struct


int main(int argc, char* args[]) {

  if ( argc < 2 ) {
    printf("RNG Example: Need at least one argument (number darts) to run; second optional argument for serial_iterations\n");
    return (-1);
  }

  Kokkos::initialize(argc,args);
  {
    const double rad = 1.0; // target radius (also box size)
    const long N = atoi(args[1]); // exponent used to create number of darts, 2^N
    const long dart_groups = argc > 2 ? atoi(args[2]) : 1 ;
    const long darts = std::pow(2,N);    // number of dart throws
    const double pi = 3.14159265358979323846; /*for reference*/ 
    printf( "Reference Value for pi:  %lf\n",pi);

    // Create a random number generator pool which takes a 64 bit unsigned integer seed to initialize a Random_XorShift generator 
    // seed is used to fill the generators of the pool.
    typedef typename Kokkos::Random_XorShift64_Pool<> RandPoolType;
    RandPoolType rand_pool(5374857);

    // Calcuate pi value for chosen sample size and generator type  
    long circHits = 0;
    Kokkos::parallel_reduce ("MC-pi-estimate", darts/dart_groups, GenRandom<RandPoolType>(rand_pool, rad, dart_groups), circHits); 

    printf( "darts = %ld  hits = %ld  pi est = %lf\n", darts, circHits, 4.0*double(circHits)/double(darts) );
  }
    Kokkos::finalize();

    return 0;
}
