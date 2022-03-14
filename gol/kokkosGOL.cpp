/* This program solves Conway's Game of Life using C/C++ and Kokkos in a way that enables 
 * the user to compile for the cpu (and execute in serial or openmp), or compile for the 
 * the gpu and execute with CUDA on one or multiple GPUs . 
 * the execution time is measured beginning from the first grid allocation to the end of the main game loop. 
 * Execution results are then displayed or written to either the console or a CSV file, via run-time parameters. 
 * Example For Console: 
 * ./kokkosGOL.exe <#grid-dimensions>
 * Example For CSV: 
 * ./serialGOL.exe <#grid-dimensions> <results.csv> 
 */ 

#include "Kokkos_Core.hpp"
#include <iostream> 
#include <chrono> /* timer */ 
#include <fstream> /* output csv */ 
#include <stdio.h>
#include <stdlib.h>
#define DEFAULT_INPUT "./kokkosGOL.cpp" 
#define SEED 1985 /* used to populate a 2D grid of 1s and 0s */ 

/* defining how to store grid using default exe space to enable cpu or gpu runs */ 
typedef Kokkos::View<double **, Kokkos::DefaultExecutionSpace> ViewMatrixType; /* since i defined it this way in serialGOL.cpp */ 

/*function prototypes*/ 
void showGrid(ViewMatrixType grid, int dim );
void showGridFull(ViewMatrixType grid, int dim);
int addNeighbors(ViewMatrixType h_A, int i, int j); 

int main(int argc, char** argv) 
{
    Kokkos::initialize(argc, argv); 
    { // start kokkos scope
	
		std::string filename = DEFAULT_INPUT; /* getting filename for output */ 
		filename = std::string(argv[0]); /* otherwise, filename is first arg*/ 
	    int dim = atoi(argv[1]);  /* square grid dimensions */  
		unsigned int generations = 1; /* number of gol iterations */ 
		
		/* checking execution space */ 
		std::cout << "\nCurrent execution space: " << 
        typeid(Kokkos::DefaultExecutionSpace).name() << "\n" << std::endl; 
			
		auto start = std::chrono::high_resolution_clock::now(); /* beginning timer */ 
	
	   /* allocating memory to two grids: A and B, contiguous memory with 0's on edges to handle boundaries */ 
		ViewMatrixType A("gridA", dim+2, dim+2);
		ViewMatrixType B("gridB", dim+2, dim+2);
		/* for performant host or device access, note: this doesn't cost anything if we only compile for cpu, just a ref */
		ViewMatrixType::HostMirror h_A = Kokkos::create_mirror_view(A); /* Note: if we only compile for cpu, deepcopy is also free */ 
		ViewMatrixType::HostMirror h_B = Kokkos::create_mirror_view(B);
		/*init grid A values of 1s or 0s (1=alive, 0=dead) */ 
		srand(SEED);
		for (int i = 1; i < dim; ++i) {
			for (int j = 1; j < dim; ++j) {
				h_A(i, j) = rand()%2; /* initialize on host */ 
			}
		}
		/* show initial gol cell values by passing view to a display function by value*/ 
		showGrid(A, dim); /*pass views by value */ 
		showGridFull(h_A, dim);
		
/* CHECKING SOLUTIONS */ 		
		/*checking 1 index to ensure it works with dim = 5*/ 
		// for (int i = 2; i<=2; i++) {
			//for (int j = 4; j<=4; j++) {
			    /* add neighbors for each grid point */ 
               // int sum_neighbors = addNeighbors(h_A, i, j);
			   // std::cout << "I am point:" << i << "," << j << "  with value:  " <<  h_A(i,j); 
			   // std::cout << "I have: " << sum_neighbors << " neighbors"; 
			//}
		//}
		
		
		/*checking to make sure it works in parallel*/ 
		//Kokkos::parallel_for("pN", dim, [=](int j)
		//{
			//for (int i = 2; i<=2; i++) {
			    /* add neighbors for each grid point */ 
             // int sum_neighbors = addNeighbors(h_A, i, j);
			 // std::cout << "I am point:" << i << "," << j << "  with value:  " <<  h_A(i,j); 
	         // std::cout << "I have: " << sum_neighbors << " neighbors"; 
			//}
		// });
/* DONE CHECKING SOLUTIONS */ 			


			
		/*running kernel in parallel*/ 
		Kokkos::parallel_for("pN", dim, [=](int j)
		{
			for (int i = 1; i<=dim; i++) {
			    /* add neighbors for each grid point */ 
                int sum_neighbors = addNeighbors(A, i, j);
			    /* GOL Rules: 
                   rule 1: living cell with < 2 live neighbors */ 
			    if (A(i,j) == 1 && sum_neighbors < 2)
                    B(i,j) = 0; /*this cell dies*/ 
				/* rule 2: living cell with 2 or 3 live neighbors */ 
				else if (A(i,j) == 1 && (sum_neighbors == 2 || sum_neighbors == 3))
                    B(i,j) = 1; /* this cell stays alive */ 
				/* rule 3: living cell with > 3 live neighbors */  
                else if (A(i,j) == 1 && sum_neighbors > 3)
                     B(i,j) = 0; /* this cell dies from overpopulation */
				/* rule 4: dead cell with 3 neighbors */ 
                else if (A(i,j) == 0 && sum_neighbors == 3)
                      B(i,j) = 1; /* birth of a new cell */ 
				  /*if none of these rules match*/ 
                else 
                    B(i,j) = A(i,j); /*original value is unchanged*/ 
			}
		});
		
		 showGridFull(B, dim);
		 /* swap grids and begin next gen calculation */ 
        ViewMatrixType tmpGrid = A;
        A = B;
        B = tmpGrid;	
	// end generations loop here
	
	 /* end main execution timer*/ 
    auto end= std::chrono::high_resolution_clock::now();
   /* store time in ms */ 
    std::chrono::duration<double, std::milli> exetime =
    end - start;
	
	  /* Sum up alive cells and print results */ 
    int still_alive = 0;
    for (int i = 1; i<=dim; i++) {
        for (int j = 1; j<=dim; j++) {
            still_alive += A(i,j); 
        }
    }
	
	std::cout << "Filename" << ',' << "Grid-Size" << ',' << "Execution-Time-ms" << ','<< "Generations" << ","  << "Total-Alive" <<  '\n';
    std::cout << filename << ',' << dim << ',' << exetime.count() << ',' << generations << ',' << still_alive << std::endl;
  
	
    }//close kokkos scope
    Kokkos::finalize(); 
    return 0; 
} 


/* PRINT FUNCTIONS */
void showGrid(ViewMatrixType grid, int dim){
	 for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
        std::cout << "  " <<  grid(i, j);
        }
	   std::cout << "\n\n"; 
     }
	  std::cout << "\n" << std::endl; 
}


/* PRINT FUNCTIONS */
void showGridFull(ViewMatrixType grid, int dim){
	 for (int i = 0; i < dim+2; ++i) {
        for (int j = 0; j < dim+2; ++j) {
        std::cout << "  " <<  grid(i, j);
        }
	   std::cout << "\n\n"; 
     }
	  std::cout << "\n" << std::endl; 
}

int addNeighbors(ViewMatrixType h_A, int i, int j){
	int sum_neighbors; 
	sum_neighbors = h_A(i+1,j) + h_A(i-1,j) + h_A(i,j+1) + h_A(i,j-1) 
	              + h_A(i+1,j+1) + h_A(i-1,j-1) + h_A(i-1,j+1) + h_A(i+1,j-1);			
		
}