// Program Description: This Program Sorts a 1-Dimensional Datatype on the CPU or the GPU
// Using Kokkos Parallel Sort and serial merge sort. 
// Purpose: To demonstrate how to use Kokkos for performance portability, to then 
// compile for and test with different backends enabled (Serial, OpenMP, Cuda, All 3, etc..)  
// Author: Thomas Gorham
// University of Tennessee at Chattanooga, Simcenter
// Date: 4/25/2022
#include <Kokkos_Core.hpp>
#include <iostream> 
#include <Kokkos_Sort.hpp> 
#include <chrono> 

typedef Kokkos::View<int*> LinearType; /* For CPU or GPU storage*/ 

//  Used to print a 1D Kokkos::View that resembles a 1D data structure, using view.extent() 
void printVector(LinearType::HostMirror vec); 
void merge(LinearType ,int, int , int ); 
void merge_sort(LinearType arr, int low, int high);

int main()
{
	Kokkos::initialize();
        {
		/*hard set for now*/ 
		int global_m = 100000;   /*size of view to sort -- can hard set or change to atoi[1] */ 	
		/* init view with random values\*/ 
                 LinearType A("A", global_m); /*for test 1*/ 
		// LinearType B("B", global_m); /*for test 2*/ 
                 LinearType::HostMirror h_A = Kokkos::create_mirror_view(A); 
                 // LinearType::HostMirror h_B = Kokkos::create_mirror_view(B); 
		  
                 for(int i = 0; i < global_m; ++i){ h_A(i) = rand() % 100;} /* fill with unsorted values */ 
                 /* for addittional testing
                 for(int i = 0; i < global_m; ++i){ h_B(i) = rand() % 100;}
                 */
		 /*print if size small*/ 
		 if (global_m < 20) {
		 std::cout << "\n\nBefore sorting:\n";
	         printVector(h_A); /* printing on host always */ 
		 }
		 Kokkos::deep_copy(A, h_A); 	
                 auto start = std::chrono::high_resolution_clock::now();
		 Kokkos::sort(A, 0, global_m);
	         auto end = std::chrono::high_resolution_clock::now(); 
		 std::chrono::duration<double, std::milli> mstime = end - start; 
                 std::cout << "\nKokkos Sort Took: " << mstime.count() << " milliseconds"; 
                 //std::chrono::duration<double, std::micro> microtime = end - start;
                 Kokkos::deep_copy(h_A, A); 
	         /* for test 2 merge sort 
		 auto start2 = std::chrono::high_resolution_clock::now();
	         merge_sort(arr2, 0, global_m-1);
	         auto end2 = std::chrono::high_resolution_clock::now(); 
	         std::chrono::duration<double, std::milli> mstime2 = end2 - start2; 
                 std::cout << "\nMerge Sort Took: " << mstime2.count() << " milliseconds"; 
	         Kokkos::deep_copy(h_arr2, arr2);
		 */	
	         /* check for accurate sorting */ 
		 if(global_m < 20) {
			 std::cout << "\n\nSorted Arrays:\n";
                         printVector(h_A);
		       //printVector(h_arr2); 
		    }
	  } /*close kokkos scope*/ 
	  Kokkos::finalize();  
} /*end main*/ 


void printVector(LinearType::HostMirror vec)
{
    std::cout << "\nVector Name: " << vec.label() << std::endl; /*label() to get view identity*/ 
    for (unsigned int i=0; i<vec.extent(0); ++i){
        std::cout<<  vec(i) << "\t";;
    }
    std::cout << "\n";
}


void merge_sort(LinearType arr, int low, int high)
{
    int mid;
    if (low < high){
        //divide the array at mid and sort independently using merge sort
        mid=(low+high)/2;
        merge_sort(arr,low,mid);
        merge_sort(arr,mid+1,high);
        //merge or conquer sorted arrays
        merge(arr,low,high,mid);
    }
}
// Merge sort 
void merge(LinearType arr, int low, int high, int mid)
{
    int i, j, k; 
    LinearType c("C",50);
    i = low;
    k = low;
    j = mid + 1;
    while (i <= mid && j <= high) {
        if (arr(i) < arr(j)) {
            c(k) = arr(i);
            k++;
            i++;
        }
        else  {
            c(k) = arr(j);
            k++;
            j++;
        }
    }
    while (i <= mid) {
        c(k) = arr(i);
        k++;
        i++;
    }
    while (j <= high) {
        c(k) = arr(j);
        k++;
        j++;
    }
    for (i = low; i < k; i++)  {
        arr(i) = c(i);
    }
}
