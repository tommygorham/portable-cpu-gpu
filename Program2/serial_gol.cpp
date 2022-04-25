/* This program solves Conway's Game of Life in serial C/C++ and calculates the execution time 
 * beginning from the first grid allocation to the end of the main game loop. Then it prints 
 * these results to either the console or a CSV file. This print is dependent upon the commands 
 * that are passed at run-time. 
 * Example For Console: 
 * ./serialGOL.exe <#grid-dimensions>
 * Example For CSV: 
 * ./serialGOL.exe <#grid-dimensions> <results.csv> 
 */ 

#include <iostream> 
#include <chrono> /* timer */ 
#include <fstream> /* output csv */ 
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_INPUT "./serialGOL.cpp" /* file name for csv output */ 
#define SEED 1985 /* used to populate a 2D grid of 1s and 0s */ 

/*function prototypes*/ 
void printGrid(int** grid, int dim); 
void printGridFullFormat(int** grid, int dim);
int addNeighbors(int** grid, int i, int j); 

int main(int argc, char* argv[])
{
    /* getting filename for output */ 
    std::string filename = DEFAULT_INPUT;
    filename = std::string(argv[0]);

    /* global vars */ 
    int dim = atoi(argv[1]);  /*grid dimensions*/  
    int generations = 1; /* number of gol iterations */ 
  
    /* begin program execution timer */ 
    auto start = std::chrono::high_resolution_clock::now();

	  /* allocating memory to two grids: grid, newGrid
	     allocate starting grid contiguously with 0's on edges to handle boundaries. 
       (dim+2)^2 elements are required for the 0s  
	   */ 
    int **grid = (int**) malloc( sizeof(int*) * (dim+2) );
	  for(int i = 0; i<dim+2; i++)
    grid[i] = (int*) malloc( sizeof(int*) * (dim+2) );

    int **newGrid = (int**) malloc( sizeof(int*) * (dim+2) );
    for(int i = 0; i<dim+2; i++){
        newGrid[i] = (int*) malloc( sizeof(int*) * (dim+2) );
	  }

	  /*init values of 1s or 0s (1=alive, 0=dead) */ 
	  srand(SEED);
	  for(int i = 1; i<=dim; i++) {
        for(int j = 1; j<=dim; j++) {
            grid[i][j] = rand()%2; 
        }
    }
	
	  // show initial grid for debug  
	  std::cout << "\nInitial grid \n" << std::endl; 
	  printGridFullFormat(grid, dim); 
	

	  /* BEGIN GAME LOOP:
	     for no wrapping, I am not copying real cols/rows to ghost col/rows 
    */ 
	  for (int current_gen = 0; current_gen < generations; ++current_gen){ /*this is the game driver*/ 
		  for (int i = 1; i<=dim; i++) {
			  for (int j = 1; j<=dim; j++) {
			          /* add neighbors for each grid point */ 
                int sumNeighbors = addNeighbors(grid, i, j);
                /* GOL Rules: 
                   rule 1: living cell with < 2 live neighbors */ 
                if (grid[i][j] == 1 && sumNeighbors < 2)
                    newGrid[i][j] = 0; /*this cell dies*/ 
                /* rule 2: living cell with 2 or 3 live neighbors */ 
                else if (grid[i][j] == 1 && (sumNeighbors == 2 || sumNeighbors == 3))
                    newGrid[i][j] = 1; /* this cell stays alive */ 
                /* rule 3: living cell with > 3 live neighbors */  
                else if (grid[i][j] == 1 && sumNeighbors > 3)
                    newGrid[i][j] = 0; /* this cell dies from overpopulation */
				        /* rule 4: dead cell with 3 neighbors */ 
                else if (grid[i][j] == 0 && sumNeighbors == 3)
                    newGrid[i][j] = 1; /* birth of a new cell */ 
                else 
                    newGrid[i][j] = grid[i][j]; /*original value is unchanged*/ 
            }
        } /* END of current generation */ 
		
		/*for debug */ 
     //std::cout << "\n The next generation grid:\n" << std::endl; 
     //printGridFullFormat(newGrid, dim); 
	    

		/* swap grids and begin next gen calculation */ 
    int **tmpGrid = grid;
    grid = newGrid;
    newGrid = tmpGrid;	
	} /* END MAIN GAME LOOP */ 
    
    /* end main execution timer*/ 
    auto end= std::chrono::high_resolution_clock::now();
    /* store time in ms */ 
    std::chrono::duration<double, std::milli> exetime =
    end - start;
  
    /* Sum up alive cells and print results */ 
    int still_alive = 0;
    for (int i = 1; i<=dim; i++) {
        for (int j = 1; j<=dim; j++) {
            still_alive += grid[i][j];
        }
    }
	
  /* THIS SECTION IS FOR RECORDING RESULTS TO CONSOLE OR CSV */ 
  if (!(argc >= 3)) {
    std::cout << "Filename" << ',' << "Grid-Size" << ',' << "Execution-Time-ms" << ','<< "Generations" << ","  << "Total-Alive" <<  '\n';
    std::cout << filename << ',' << dim << ',' << exetime.count() << ',' << generations << ',' << still_alive << std::endl;
  }
  
  else {
    std::string file_path(argv[2]);
    std::ofstream csv_output;
    std::ifstream input_file;
    input_file.open(file_path);
    
    // If file_path does not refer to an existing file
    if (!input_file) { 
      csv_output.open(file_path);
      if (!csv_output.is_open())
        return -1; 
      csv_output << "Filename" << ',' << "Grid-Size" << ',' << "Execution-Time-ms" << ',' << "Generations" << "," << "Total-Alive" <<  '\n';
      csv_output << filename << ',' << dim << ',' << exetime.count() << ','<< generations << ',' << still_alive << std::endl;
	  csv_output.close();
    }
    // file_path does refer to an existing file
    else {
      std::string line;
      std::string answer;
      std::getline(input_file, line);
      // Checking to see if the header is matching our output
      if (line != "Filename,Grid-Size,Execution-Time-ms,Generations,Total-Alive") {
        std::cerr << "Error: Header does not match format\nDo you want to append output?\n(yes/no): ";
        // Asking user to specify whether or not they want to continue printing
        // this value to a file with an incompatible header
        std::cin >> answer;
        if (answer != "yes") {
          return 0;
        }
      }
      // Closing input file, opening output file (which should be the same) and
      // appending results to the end
      input_file.close();
      csv_output.open(file_path, std::ios_base::app);
      csv_output << filename << ',' << dim << ',' << exetime.count() << ',' << generations << ',' << still_alive << std::endl;
      csv_output.close();
    }
  } /* DONE RECORDING RESULTS TO CONSOLE OR CSV */ 
		
	/* releasing heap allocated memory */ 
	free(grid); 
	free(newGrid); 
	
	return 0; 
} /*end main*/

//############################# FUNCTION DEFINITIONS #############################
/* PRINT FUNCTIONS */
// used for normal printing/debugging   
void printGrid(int** grid, int dim) {
for(int i = 1; i<=dim; i++) {
        for(int j = 1; j<=dim; j++) {
            std::cout << grid[i][j] << " ";
        }
		std::cout << "\n";
    }
} 

// used for showing the padding/edge cases
void printGridFullFormat(int** grid, int dim) {
    printf("\n");
    for (int i = 0; i < dim+2; i++) {
      printf("\t");
      for (int j = 0; j < dim+2; j++) {
        printf("%d  ", grid[i][j]);
      }
      printf("\n\n");
    }
    printf("\n");
  }
  
  // used to add the 8 neighbors of each index in the 2d grid 
int addNeighbors(int** grid, int i, int j){ 
    int sum_neighbors;
    sum_neighbors = grid[i+1][j] + grid[i-1][j] + grid[i][j+1] + grid[i][j-1] 
	              + grid[i+1][j+1] + grid[i-1][j-1] + grid[i-1][j+1] + grid[i+1][j-1];
 
	return sum_neighbors; /* return sum for each index */  
  }
