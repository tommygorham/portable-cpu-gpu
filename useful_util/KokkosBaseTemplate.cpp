#include "Kokkos_Core.hpp" 
#include <iostream> 


int main(int argc, char** argv)
{ 
    Kokkos::initialize(argc, argv); 
    { /* start kokkos scope */ 
	
	   /* will trinkle down in this order depended on what was compiled for	    * CUDA, OPENMP, Serial 
	   */ 
	   std::cout << "\nDefault execution space: " << typeid(Kokkos::DefaultExecutionSpace).name() << "\n" << std::endl; 
	   std::cout << "\nDefault Host execution space: " << typeid(Kokkos::DefaultHostExecutionSpace).name() << "\n" << std::endl;    
		
       //this ensures the view is going to execute in the space we set at compile time  
       using viewtype = Kokkos::View<double*> Kokkos::DefaultExecutionSpace; 

    } /* end kokkoks scope */ 
    Kokkos::finalize(); 
  
   return 0;
} /* close main */ 


