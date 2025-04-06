// Hybrid Kokkos + MPI between nodes, NVIDIA GPU on node

#include "Kokkos_Core.hpp"
#include <iostream>
#include <mpi.h>

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);         // init mpi before kokkos
    Kokkos::initialize(argc, argv); // start kokkos scope
    {
        /* global vars */
        int size, world_rank, row_rank, col_rank; /* for mpi */
        double start_init, end_init, final_init, start_yAx, end_yAx,
               yAx_time_to_sol, start_redistribute, end_redistribute,
               redistribute_time;
        const int M = atoi(argv[1]); /* global rows  */
        const int N = atoi(argv[2]); /* global cols  */
        const int P = atoi(argv[3]); /* process rows */
        const int Q = atoi(argv[4]); /* process cols */

        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); 
        MPI_Comm_size(MPI_COMM_WORLD, &size); 

        /* dims */
        int MdivP = M / P;
        int NdivQ = N / Q;
        int MmodP = M % P;
        int NmodQ = N % Q;
        int local_row = world_rank / Q; // 
        int local_col = world_rank % Q; // 

        // create 2 new communicators for local 2d domain (local matrix)
        MPI_Comm row_comm;
        MPI_Comm col_comm;

        // each process calculate local matrix dims
        MPI_Comm_split(MPI_COMM_WORLD, local_row, world_rank, &row_comm);
        MPI_Comm_split(MPI_COMM_WORLD, local_col, world_rank, &col_comm);
        // initialize local dims
        int m, n; 
        if (local_row < P - 1) {
          m = MdivP;
        } 
        else {
          m = MdivP + MmodP;
        } 
        if (local_col < Q - 1) {
          n = NdivQ;
        } 
        else {
          n = NdivQ + NmodQ;
        }
        // determine the rank in row_comm
        MPI_Comm_rank(row_comm, &row_rank);
        // determine the rank in col comm
        MPI_Comm_rank(col_comm, &col_rank);

        start_init = MPI_Wtime();
        // create data structures
        using ExecSpace =
            Kokkos::Cuda; /* logical grouping of computation units which share
                             an identical set of performance properties. An
                             execution space provides a set of parallel execution
                             resources which can be utilized by the programmer using
                             several types of fundamental parallel operation. */

        using MemSpace =
            Kokkos::CudaSpace; /* The multiple types of memory which will become
                                  available in future computing nodes are abstracted
                                  by Kokkos through memory spaces. Each memory space
                                  provides a finite storage capacity at which data
                                  structures can be allocated and accessed.
                                  Different memory space types have different
                                  characteristics with respect to accessibility from
                                  execution spaces as well as their performance
                                  characteristics. */
        using Layout = Kokkos::LayoutRight; /* col major for gpu */
        using range_policy = Kokkos::RangePolicy<ExecSpace>;
        using ViewVectorType = Kokkos::View<double *, Layout, MemSpace>;
        using ViewMatrixType = Kokkos::View<double **, Layout, MemSpace>;
        ViewVectorType y("y", m);    /* sol vector */
        ViewVectorType x("x", n);    // vector x in eq. y=Ax
        ViewMatrixType A("A", m, n); // matrix A in eq. y=Ax
        ViewMatrixType ID_A("ID_A", m, n);

        // Create host mirrors of device views.
        ViewVectorType::HostMirror h_y = Kokkos::create_mirror_view(y);
        ViewVectorType::HostMirror h_x = Kokkos::create_mirror_view(x);
        ViewMatrixType::HostMirror h_A = Kokkos::create_mirror_view(A);
        ViewMatrixType::HostMirror h_ID_A = Kokkos::create_mirror_view(ID_A);
        
        // Initialize A matrix on host.
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                h_A(i, j) = 1;
            }
        }

        // Initialize vector x on host ( mpiranks in row comm)
        if (local_row == 0) {
          for (int i = 0; i < n; ++i) {
            h_x(i) = 1;
          }
        }

        /* Init ID MATRIX */
        int i_increment = M / P; /* every rank increment this many rows globally */
        int j_increment = N / Q; /* every rank increment this many cols globally */
        /* fill id matrix by finding glocal (i,j) */
        for (int local_i = 0; local_i < m; ++local_i) {
            for (int local_j = 0; local_j < n; ++local_j) {
                int global_i = (i_increment * local_row + local_i);
                int global_j = (j_increment * local_col + local_j);
                if (global_i == global_j) {
                    h_ID_A(local_i, local_j) = 1;
                } else
                    h_ID_A(local_i, local_j) = 0;
             }
        }
        MPI_Bcast(h_x.data(), n, MPI_DOUBLE, 0, col_comm);
        
        // deep copy and launch kernel
        Kokkos::deep_copy(y, h_y);
        Kokkos::deep_copy(x, h_x);
        Kokkos::deep_copy(A, h_A);
        end_init = MPI_Wtime(); 
        final_init = end_init - start_init;

        // start yax
        start_yAx = MPI_Wtime();
        double result = 0;
        Kokkos::parallel_reduce("yAx", m, KOKKOS_LAMBDA(int i, double &update) {
            double temp2 = 0;
            for (int j = 0; j < n; ++j) {
                temp2 += A(i, j) * x(j);
            }
            update = temp2;
            y(i) = update;
        }, result);
        Kokkos::fence();
        Kokkos::deep_copy(h_y, y); // copy back to host fom device
        
        // use row communicator to retrieve value for sum horizontally
        if (local_col == 0) {
            MPI_Reduce(MPI_IN_PLACE, h_y.data(), m, MPI_DOUBLE, MPI_SUM, 0, row_comm);
            // local col 0 now has correct solution for y
        } else {
            MPI_Reduce(h_y.data(), NULL, m, MPI_DOUBLE, MPI_SUM, 0, row_comm);
        }
        
        // broadcast so all processes store y
        MPI_Bcast(h_y.data(), m, MPI_DOUBLE, 0, row_comm);
        end_yAx = MPI_Wtime();
        
        // calculate computation time 
        yAx_time_to_sol = end_yAx - start_yAx;

        /* ****************** Compute xIDy*********************** */
        start_redistribute = MPI_Wtime();
        deep_copy(y, h_y);       // device needs updated y vals
        deep_copy(ID_A, h_ID_A); // copy ID matrix to device
        double idresult = 0;
        Kokkos::parallel_reduce("xIDy", n, KOKKOS_LAMBDA(int i, double &update) {
            double temp2 = 0;
            for (int j = 0; j < m; ++j) {
                temp2 += ID_A(i, j) * y(i);
            }
            update = temp2;
            x(i) = update;
        }, idresult);
        Kokkos::fence();
        Kokkos::deep_copy(h_x, x); // deep copy back to host
        MPI_Allreduce(MPI_IN_PLACE, h_x.data(), n, MPI_DOUBLE, MPI_SUM, col_comm);
        end_redistribute = MPI_Wtime();
        redistribute_time = end_redistribute - start_redistribute;
        std::cout << "\nRank: " << world_rank << "\ninit_time: " << final_init
                  << "\nyAx: " << yAx_time_to_sol
                  << "\nRedistribute y: " << redistribute_time << std::endl;
        } // end scope for Kokkos::initialize
        Kokkos::finalize();
        MPI_Finalize();

  return 0;
}
