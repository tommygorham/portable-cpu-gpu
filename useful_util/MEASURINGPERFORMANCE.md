# Timing with Linux 

The Linux "time" command measures how long a command takes to execute. 
This can be useful to pass at runtime for measuring how long the
entire program execution takes. 

`time ./exe`


# Timing with C++ 
Chrono High Resolution Clock seems like the best choice from the standard library. 
```c++
#include <chrono> 
int main()
{
  ...
  auto start = std::chrono::high_resolution_clock::now(); 
  /* expensive computation here */ 
  auto end = std::chrono::high_resolution_clock::now();
  // report in milliseconds 
  std::chrono::duration<double, std::milli> mstime = end - start; 
  std::cout << "\nDot Product Took: " << mstime.count() << " milliseconds"; 
  // report in microseconds 
  std::chrono::duration<double, std::micro> microtime = end - start;
  std::cout << "  or, " << microtime.count() << " microseconds" << std::endl;
}
```
# Timeing with Openmp  
omp_get_wtime() returns time in seconds 
```c++ 
#include <omp.h> 
int main()
{ 
  ... 
  double start, end, ms; 
  // serial example
  omp_set_num_threads(1); 
  start = omp_get_wtime(); 
  /* expensive computation here */ 
  end = omp_get_wtime();  
  ms = (end-start)*1000.0; 
  cout << "\ntime in milliseconds: " << ms << endl;
}
```

# Timing with MPI 
MPI_Wtime() returns time in seconds 
```c++ 
#include <mpi.h>
int main()
{
  ...
  double start; 
  MPI_Barrier(MPI_COMM_WORLD); // sync before timing 
  start = MPI_Wtime(); 
  /* expensive computation here */ 
  MPI_Barrier(MPI_COMM_WORLD); // ensure every process has finished computation 
  double time = MPI_Wtime() - start; // via pg. 105 of "Using MPI, 3rd Edition"
}
```
# Writing Results to a CSV 
A colleague and I wrote this in April 2021, so that we could append to a csv if it existed, or create a new one as needed
```c++ 
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#define DEFAULT_INPUT "./myprogram.cpp"

int main(int argc, char** argv) 
{
  const int PROBLEMSIZE = atoi(argv[1]); // just an example
  // filename for output
  std::string filename = DEFAULT_INPUT;
  filename = std::string(argv[0]);
  
  /* computations */ 
  ... 
  
  // prepare for .csv if provided
  std::string file_path(argv[2]);
  std::ofstream csv_output;
  std::ifstream input_file;
  input_file.open(file_path);  
  
  // If file_path does not refer to an existing file
  if (!input_file) 
  { 
      csv_output.open(file_path);
      if (!csv_output.is_open())
        return -1; 
      csv_output << "Filename" << ','
      csv_output << filename << ',' << N << ',' << allocation_time.count() << ',' << multiplication_time.count() << std::endl;
      csv_output.close();
  } 
    // file_path does refer to an existing file
  else 
  {
      std::string line;
      std::string answer;
      std::getline(input_file, line);
      // Checking to see if the header is matching our output
      if (line != "Filename,Matrix-size,Allocation-time,Multiplication-time") 
      {
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
      csv_output << filename << ',' << N << ',' << allocation_time.count() << ',' << multiplication_time.count() << std::endl;
      csv_output.close();
   }
   
   return 0; 
   
} // end main 
``` 
