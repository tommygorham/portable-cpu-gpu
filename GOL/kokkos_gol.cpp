/* PROGRAM DESCRIPTION: This program solves Conway's Game of Life using C/C++ and Kokkos. 
 * HOW TO RUN: ./kokkos_gol <num-grid-dimensions> <results.csv> 
 */ 
#include "Kokkos_Core.hpp"
#include <iostream> 
#include <chrono>  // timer 
#include <fstream> // output csv  
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_INPUT "./kokkosGOL.cpp" 
#define SEED 1985 /* used to populate a 2D grid of 1s and 0s */ 

/* Defining grid using default exe space to enable cpu or gpu runs without refactoring any code */ 
typedef Kokkos::View<int **, Kokkos::DefaultExecutionSpace> ViewMatrixType;

void showGrid(ViewMatrixType::HostMirror grid, int dim );
void showGridFull(ViewMatrixType::HostMirror grid, int dim); 
int stillAlive(ViewMatrixType::HostMirror, int dim);  
int addNeighbors(ViewMatrixType grid, int i, int j); 

int main(int argc, char** argv) 
{
    Kokkos::initialize(argc, argv); 
    {   // start kokkos scope
        std::string filename = DEFAULT_INPUT; /* getting filename for outputting performance results */ 
        filename = std::string(argv[0]);      /* otherwise, filename is first arg*/ 
        int dim = atoi(argv[1]);              /* square grid dimensions */  
        unsigned int generations = 1000;      /* number of gol iterations */ 
        std::cout << "\nCurrent execution space: " << 
        typeid(Kokkos::DefaultExecutionSpace).name() << "\n" << std::endl; 
    
        /* Allocate memory to two grids: A and B, contiguous memory with 0's on edges to handle boundaries */ 
        ViewMatrixType A("gridA", dim+2, dim+2);
        ViewMatrixType B("gridB", dim+2, dim+2);
        ViewMatrixType tmp("gridtmp", dim+2, dim+2);

        /* For host or device access, note: this doesn't cost anything if we only compile for cpu, just a ref */
        ViewMatrixType::HostMirror h_A = Kokkos::create_mirror_view(A);  
        ViewMatrixType::HostMirror h_B = Kokkos::create_mirror_view(B);
        ViewMatrixType::HostMirror h_tmp = Kokkos::create_mirror_view(tmp);
       
        /* Init grid A values of 1s or 0s on host (where 1=alive, 0=dead) */ 
        srand(SEED);
        for (int i = 1; i < dim; ++i) {
            for (int j = 1; j < dim; ++j) {
                h_A(i, j) = rand()%2; 
            }
        }
        /* Show initial gol cell values by passing view to a display function by value*/ 
        showGridFull(h_A, dim);
        int initial_alive = stillAlive(h_A, dim); 
        std::cout << "\n Initial Cells Alive: " << initial_alive; 

        /* If GPU -> copy for processing, else this is a no op */ 
        Kokkos::deep_copy(A, h_A);
        Kokkos::deep_copy(B, h_B);  
        Kokkos::deep_copy(tmp, h_tmp);  
         
        // Starting kernel and timer
        auto start = std::chrono::high_resolution_clock::now();  
        // GOL is based on chronological iterations
        for (unsigned int a = 0; a < generations; ++a)    
        {
            Kokkos::parallel_for(dim, KOKKOS_LAMBDA(int j) // Make expensive calculations parallel   
            { 
                // count dead/alive cells from 8 neighbors 
                int sum_neighbors=0; 
                for (int i = 1; i<=dim; i++)
                {
                    sum_neighbors = A(i+1,j) + A(i-1,j) + A(i,j+1) + A(i,j-1) + A(i+1,j+1) + A(i-1,j-1) + A(i-1,j+1) + A(i+1,j-1);          
                    // Per assignment directions, no wrapping; rule 1: living cell with < 2 live neighbors  
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
             }); 
             //swap grids and send back through parallel construct  
             tmp = A;
             A = B;
             B = tmp;  
        } // end generations loop here
        auto end = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double, std::milli> goltime =
        end - start; 
        // copy back to host if gpu enabled and we need to print final state
        Kokkos::deep_copy(h_A, A); 
        std::cout << "\n\nGrid after " << generations << " generations\n"; 
        /* Sum up alive cells and print results */ 
        int still_alive = stillAlive(h_A, dim); 
        std::cout << "\nCells Still Alive: " << still_alive; 
        std::cout << "Filename" << ',' << "Grid-Size" << ',' << "Execution-Time-ms" << ','<< "Generations" << ","  << "Total-Alive" <<  '\n';
        std::cout << filename << ',' << dim << ',' << goltime.count() << ',' << generations << ',' << still_alive << std::endl;
        } // close kokkos scope
        Kokkos::finalize(); 
        return 0; 
} 

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

// Display the grid with the 0s padding. use this one to see whole picture 
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

// Calculates the total number of cells still alive
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
