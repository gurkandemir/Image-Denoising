// Author: Burak Suyunu
// Date: 05.12.2018
// CmpE300 MPI PS
//
// MPI_Send, MPI_Recv example.
// Root (0) process sends 2 rows of 2NxN 2D array to slave processes
// using DYNAMIC memory allocation
//
// NOT WORKING (INTENTIONAL) (see send_recv_array4.c and send_recv_array5.c for solution)
//
// Because All MPI data copy routines expect that source and destination memory are "flat" linear arrays.
// Multidimensional C arrays should be stored in row major order not in arrays of pointers.
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);


  int N = world_size-1;
  int i, j;

  if (world_rank == 0) {
    printf("N: %d\n", N);
    int** arr = NULL;
    arr = (int **)malloc(sizeof(int*)*2*N);
    for(i = 0 ; i < 2*N ; i++)
      arr[i] = (int *)malloc(sizeof(int) * N);

    int count = 0;
    for(i = 0 ; i < 2*N ; i++)
      for(j = 0 ; j < N ; j++)
        arr[i][j] = ++count;


    for(i = 0 ; i < 2*N ; i++){
      for(j = 0 ; j < N ; j++){
        printf("%d ", arr[i][j]);
      }
      printf("\n");
    }
   

    for(i = 1 ; i <= N ; i++)
      MPI_Send(arr[2*(i-1)], 2*N, MPI_INT, i, 0, MPI_COMM_WORLD);
  }
  else{
    int* subarr = NULL;
    subarr = (int *)malloc(sizeof(int) * 2 * N);
    MPI_Recv(subarr, 2*N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process %d received elements: ", world_rank);
    for(i = 0 ; i < 2*N ; i++)
      printf("%d ", subarr[i]);
    printf("\n");
  }
  MPI_Finalize();
}
