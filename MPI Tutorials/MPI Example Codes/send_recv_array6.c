// Author: Burak Suyunu
// Date: 05.12.2018
// CmpE300 MPI PS
//
// MPI_Send, MPI_Recv example.
// Root (0) process sends 2 rows of 2NxN 2D array to slave processes
// using DYNAMIC memory allocation in a COOL way.
// 
// 4. method in this site
// https://www.geeksforgeeks.org/dynamically-allocate-2d-array-c/
//
// All MPI data copy routines expect that source and destination memory are "flat" linear arrays.
// 
// COOL SOLUTION: There was actually such a way to DYNAMICALLY allocate the 2D arary such that
//                the 2D array stored as row major order.
//
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

    int r = 2*N;
    int c = N;
    int *ptr, **arr;
    int len;
    len = sizeof(int *) * r + sizeof(int) * c * r;
    arr = (int **)malloc(len);

    // ptr is now pointing to the first element in of 2D array
    ptr = arr + r;

    // for loop to point rows pointer to appropriate location in 2D array
    for(i = 0; i < r; i++)
      arr[i] = (ptr + c * i);



    int count = 0;
    for(i = 0 ; i < r ; i++)
      for(j = 0 ; j < c ; j++)
        arr[i][j] = ++count;

    for(i = 0 ; i < r ; i++){
      for(j = 0 ; j < c ; j++){
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
