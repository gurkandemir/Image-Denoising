// Author: Burak Suyunu
// Date: 05.12.2018
// CmpE300 MPI PS
// 
// This code illustrates different memory allocation methods for 2D arrays.
// The code prints out the memory locations of each array components to show
// how the memory is allocated.
// Comment out the method you want to see.
// 
// Method 1: Standard fully DYNAMIC memory allocation for 2D array
// Method 2: Cool way of DYNAMIC memory allocation for 2D array
// Method 3: STATIC memory allocation for 2D array
//
// Compile:
// gcc -g mem_allocate_2d.c -o mem_allocate_2d
//
// Run:
// ./mem_allocate_2d
//
//
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  // Initialize the MPI environment

  int i, j, r = 3, c = 4;
  printf("r: %d - c: %d\n\n", r, c);


  // METHOD 1 - send_recv_array3.c
  // Standard fully dynamic memory allocation for 2D array
  // int** arr = NULL;
  // arr = (int **)malloc(sizeof(int*)*r);
  // for(i = 0 ; i < r ; i++)
  //   arr[i] = (int *)malloc(sizeof(int) * c);


  // METHOD 2 - send_recv_array6.c
  // Cool way of DYNAMIC memory allocation for 2D array
  // int *ptr, **arr;
  // int len;
  // len = sizeof(int *) * r + sizeof(int) * c * r;
  // arr = (int **)malloc(len);

  // // ptr is now pointing to the first element in of 2D array
  // ptr = arr + r;

  // // for loop to point rows pointer to appropriate location in 2D array
  // for(i = 0; i < r; i++)
  //   arr[i] = (ptr + c * i);


  // METHOD 3 - send_recv_array4.c
  // STATIC memory allocation for 2D array
  int arr[r][c];

  

  int count = 0;
  for(i = 0 ; i < r ; i++){
    for(j = 0 ; j < c ; j++){
      arr[i][j] = ++count;
    }
  }

  printf("&arr: %p - arr: %p\n\n", &arr, arr);
  for(i = 0 ; i < r ; i++){
    printf("&arr[%d]: %p - arr[%d]: %p\n", i, &arr[i], i, arr[i]);
    for(j = 0 ; j < c ; j++){
      printf("&arr[%d][%d]: %p - ", i, j, &arr[i][j]);
    }
    printf("\n\n");
  }
   

}
