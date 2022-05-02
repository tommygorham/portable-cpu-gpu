/* PROGRAM DESCRIPTION: This program solves Conway's Game of Life using C/C++ and Kokkos. 
 *  Purpose: Enables the user to compile for the cpu (and execute in serial or parallel with openmp), or 
 * compile for the gpu and execute with CUDA/CUDAUVM, HIP, OpenCL, etc.. on a GPU. 
 * the execution time has been updated to measures main game loop.  
 * Execution results are then displayed or written to either the console or a CSV file, via run-time parameters. 
 * Example For Console: 
 * ./kokkosGOL.exe <#grid-dimensions>
 * Example For CSV: 
 * ./serialGOL.exe <#grid-dimensions> <results.csv> 
 * Author: Thomas Gorham 
 * Good Reference for serial or CUDA version: ORNL Conway's Game of Life Tutorial 
 */ 

#include "Kokkos_Core.hpp"
#include <iostream> 
#include <chrono> /* timer */ 
#include <fstream> /* output csv */ 
#include <stdio.h>
#include <stdlib.h>
#define DEFAULT_INPUT "./kokkosGOL.cpp" 

#define SEED 1985 /* used to populate a 2D grid of 1s and 0s */ 

/* defining how to store grid using default exe space to enable cpu or gpu runs without refactoring any code */ 
typedef Kokkos::View<int **, Kokkos::DefaultExecutionSpace> ViewMatrixType; /* since i defined it this way in serialGOL.cpp */ 

/*function prototypes*/ 
void showGrid(ViewMatrixType::HostMirror grid, int dim );
void showGridFull(ViewMatrixType::HostMirror grid, int dim); 
int stillAlive(ViewMatrixType::HostMirror, int dim);  
int addNeighbors(ViewMatrixType grid, int i, int j); /*this one is in the process of being depreciated*/ 

int main(int argc, char** argv) 
{
	Kokkos::initialize(argc, argv); 
        { // start kokkos scope
		std::string filename = DEFAULT_INPUT; /* getting filename for outputting performance results */ 
		filename = std::string(argv[0]); /* otherwise, filename is first arg*/ 
                int dim = atoi(argv[1]);  /* square grid dimensions */  
		unsigned int generations = 1; /* number of gol iterations */ 
		
	         /* just checking execution space that I have been setting via compile-time */ 
	        std::cout << "\nCurrent execution space: " << 
                typeid(Kokkos::DefaultExecutionSpace).name() << "\n" << std::endl; 
	
                /* allocating memory to two grids: A and B, contiguous memory with 0's on edges to handle boundaries */ 
		ViewMatrixType A("gridA", dim+2, dim+2);
		ViewMatrixType B("gridB", dim+2, dim+2);
	        ViewMatrixType tmp("gridtmp", dim+2, dim+2);

                /* for performant host or device access, note: this doesn't cost anything if we only compile for cpu, just a ref */
		ViewMatrixType::HostMirror h_A = Kokkos::create_mirror_view(A); /* Note: if we only compile for cpu, deepcopy is also free (essentially a no op) */ 
		ViewMatrixType::HostMirror h_B = Kokkos::create_mirror_view(B);
		ViewMatrixType::HostMirror h_tmp = Kokkos::create_mirror_view(tmp);
       
                /*init grid A values of 1s or 0s on host (where 1=alive, 0=dead) */ 
	        srand(SEED);
	        for (int i = 1; i < dim; ++i){
			for (int j = 1; j < dim; ++j) {
				h_A(i, j) = rand()%2; /* initialize on host */ 
			}
		}
		/* show initial gol cell values by passing view to a display function by value*/ 
		showGridFull(h_A, dim);
                int initial_alive = stillAlive(h_A, dim); 
		std::cout << "\n Initial Cells Alive: " << initial_alive; 

                /* if GPU -> copy for processing, else this is a no op */ 
                Kokkos::deep_copy(A, h_A);
                Kokkos::deep_copy(B, h_B);  
		Kokkos::deep_copy(tmp, h_tmp); 	
		 
		// starting kernel and timer
                auto start = std::chrono::high_resolution_clock::now(); /* beginning timer */ 
                for(unsigned int a = 0; a < generations; ++a) /*since this game is based on chronological iterations*/ 
                {
			Kokkos::parallel_for(dim, KOKKOS_LAMBDA(int j) /* making expensive calculations parallel */  
		        { 
				int sum_neighbors=0; /* count dead/alive cells from 8 neighbors */ 
			        for (int i = 1; i<=dim; i++) {
					//add neighbors for each grid point, removed fuction implemented in serial_gol.cpp... gives gpu trouble 
                                        sum_neighbors = A(i+1,j) + A(i-1,j) + A(i,j+1) + A(i,j-1) + A(i+1,j+1) + A(i-1,j-1) + A(i-1,j+1) + A(i+1,j-1);			
				        // Per assignment directions, no wrapping 
			                // GOL Rules: 
                                        // rule 1: living cell with < 2 live neighbors  
			                if (A(i,j) == 1 && sum_neighbors < 2)
                                            B(i,j) = 0; //this cell dies 
				        // rule 2: living cell with 2 or 3 live neighbors  
				        else if (A(i,j) == 1 && (sum_neighbors == 2 || sum_neighbors == 3))
                                                 B(i,j) = 1; // this cell stays alive  
				        // rule 3: living cell with > 3 live neighbors 
                                        else if (A(i,j) == 1 && sum_neighbors > 3)
                                                 B(i,j) = 0; // this cell dies from overpopulation 
				        // rule 4: dead cell with 3 neighbors 
                                        else if (A(i,j) == 0 && sum_neighbors == 3)
                                                 B(i,j) = 1; // birth of a new cell
				        // if none of these rules match 
                                        else 
                                        B(i,j) = A(i,j); //original value is unchanged
                               }
		        }); //end par for 
  
                      //swap grids and send back through parallel construct  
                      tmp = A;
                      A = B;
                      B = tmp;	
               } // end generations loop here
       	 
        auto end = std::chrono::high_resolution_clock::now(); /* end gol execution timer*/ 
        /* store time in ms */ 
        std::chrono::duration<double, std::milli> goltime =
        end - start; 
        // copy back to host if gpu enabled and we need to print final state
        Kokkos::deep_copy(h_A, A); /* no op if gpu is not enabled */ 
        std::cout << "\n\nGrid after " << generations << " generations\n"; 
	      /* Sum up alive cells and print results */ 
        std::cout << "\nCells Still Alive: " << stillAlive(h_A, dim); 
	   
	// TO DO: Add file output handling implemented in Program 1
        /* for now, just uncomment these two lines for all output results */ 
		    //std::cout << "Filename" << ',' << "Grid-Size" << ',' << "Execution-Time-ms" << ','<< "Generations" << ","  << "Total-Alive" <<  '\n';
		    //std::cout << filename << ',' << dim << ',' << exetime.count() << ',' << generations << ',' << still_alive << std::endl;
  
        }//close kokkos scope
        Kokkos::finalize(); 
        return 0; 
} 

//############################# FUNCTION DEFINITIONS #############################

/* PRINT FUNCTIONS */
void showGrid(ViewMatrixType::HostMirror grid, int dim){
	 std::cout << "\nGrid: " << grid.label() << "\n"; 
     for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
        std::cout << "  " <<  grid(i, j);
        }
	   std::cout << "\n\n"; 
     }
	  std::cout << "\n" << std::endl; 
}

// this shows the grid with the 0s padding. use this one to see whole picture 
void showGridFull(ViewMatrixType::HostMirror grid, int dim){
	 std::cout << "\nGrid: " << grid.label() << "\n"; 
     for (int i = 0; i < dim+2; ++i) {
        for (int j = 0; j < dim+2; ++j) {
        std::cout << "  " <<  grid(i, j);
        }
	   std::cout << "\n\n"; 
     }
	  std::cout << "\n" << std::endl; 
}

//calculates the total number of cells still alive
/* TO DO: make kokkos::parallel_for */  
int stillAlive(ViewMatrixType::HostMirror h_A, int dim)
{
    int cells_still_alive = 0;
    for (int i = 1; i<=dim; i++) {
        for (int j = 1; j<=dim; j++) {
            cells_still_alive += h_A(i,j); 
        }
    } 
    return cells_still_alive; 
}
