/* Program Description: This Program Sorts a 1-Dimensional Datatype on the CPU or the GPU using Kokkos Parallel Sort 
 * and serial merge sort. 
 * University of Tennessee at Chattanooga, Simcenter
 * Date: 4/25/2022
 */ 
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
        // Size of data to sort
        int global_m = 100000;  
        // Init view with random values
         LinearType A("A", global_m); /*for test 1*/ 
         LinearType::HostMirror h_A = Kokkos::create_mirror_view(A); 
         for (int i = 0; i < global_m; ++i){ h_A(i) = rand() % 100;} /* fill with unsorted values */ 
         /*print if size small*/ 
         if (global_m < 20) {
             std::cout << "\n\nBefore sorting:\n";
             printVector(h_A); 
         }
         Kokkos::deep_copy(A, h_A);     
         auto start = std::chrono::high_resolution_clock::now();
         Kokkos::sort(A, 0, global_m);
         auto end = std::chrono::high_resolution_clock::now(); 
         std::chrono::duration<double, std::milli> mstime = end - start; 
         std::cout << "\nKokkos Sort Took: " << mstime.count() << " milliseconds"; 
         Kokkos::deep_copy(h_A, A); 
         /* Check for accurate sorting */ 
         if (global_m < 20) {
             std::cout << "\n\nSorted Arrays:\n";
             printVector(h_A);
         }
    } /*close kokkos scope*/ 
    Kokkos::finalize();  
} 

void printVector(LinearType::HostMirror vec)
{
    std::cout << "\nVector Name: " << vec.label() << std::endl;  
    for (unsigned int i=0; i<vec.extent(0); ++i){
        std::cout<<  vec(i) << "\t";;
    }
    std::cout << "\n";
}

void merge_sort(LinearType arr, int low, int high)
{
    int mid;
    if (low < high){
        // Divide the array at mid and sort independently using merge sort
        mid=(low+high)/2;
        merge_sort(arr,low,mid);
        merge_sort(arr,mid+1,high);
        merge(arr,low,high,mid);
    }
}

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
