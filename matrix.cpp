#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>

#define mA 4
#define nA 2
#define mB 2
#define nB 5
#define num_threads 2

int **A;
int **B;
int **C;

int ** allocate(int **matrix, int m, int n) {
  matrix = (int**) malloc(m * sizeof(int*));
  matrix[0] = (int*) malloc(m * n * sizeof(int));

  for (int i = 1; i < m; i++) {
    matrix[i] = matrix[0] + i * n;
  }

  return matrix;
}


void value(int **matrix, int row, int column) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      matrix[i][j] = rand() % 6;
    }
  }
}


void print_matrix(int **matrix, int row, int column) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      printf("%d ", matrix[i][j]);
      if (j == column - 1) {
        printf("\n");
      }
    }
  }
}


void *mul(void *num_hilo) {
  int tid, portion_size, row_start, row_end, suma;

  tid = *(int *)(num_hilo);
  portion_size = mA / num_threads;

  if (portion_size == 0) {
    for (int i = 0; i < mA; i++) {
      for (int j = 0; j < nB; j++) {
        suma = 0;
        for (int k = 0; k < mB; k++) {
          //printf("\nA[%d][%d] -> %d - B[%d][%d] -> %d \n", i, k, A[i][k], k, j, B[k][j]);
          suma += A[i][k] * B[k][j];
        }
        C[i][j] = suma;
      }
    }
  }

  else {
    row_start = tid * portion_size;
    if(tid == (num_threads - 1)) {
      row_end = mA;
    }
    else {
        row_end = (tid + 1) * portion_size;
    }
    //printf("thread: %d\nportion size: %d\n", tid, portion_size);
    //printf("row_start: %d\nrow_end: %d\n", row_start, row_end);

    for (int i = row_start; i < row_end; i++) {
      for (int j = 0; j < nB; j++) {
        suma = 0;
        for (int k = 0; k < mB; k++) {
          //printf("A[%d][%d] -> %d - B[%d][%d] -> %d \n", i, k, A[i][k], k, j, B[k][j]);
          suma += A[i][k] * B[k][j];
        }
        C[i][j] = suma;
      }
    }
  }
	return nullptr;
}


int main() {
  struct timeval start;
	struct timeval finish;
	long compTime;
	double Time = 0;

  gettimeofday(&start, 0);
  srand(time(NULL));

  pthread_t * threads;

  threads = (pthread_t *) malloc( num_threads * sizeof(pthread_t) );

  if (nA != mB) {
    printf("ERROR\n");
    return 1;
  }

  A = allocate(A, mA, nA);
  B = allocate(B, mB, nB);
  C = allocate(C, mA, nB);

  value(A, mA, nA);
  value(B, mB, nB);

  print_matrix(A, mA, nA);
  printf("\n");
  print_matrix(B, mB, nB);

  printf("\n");
  for (int i = 0; i < num_threads; i++) {
    int *tid;
    tid = (int *) malloc( sizeof(int) );
    *tid = i;
    pthread_create(&threads[i], NULL, mul, (void*)tid);
  }

  for (int j = 0; j < num_threads; j++) {
    pthread_join(threads[j], NULL);
  }

  print_matrix(C, mA, nB);

  gettimeofday(&finish, 0);

  compTime = (finish.tv_sec - start.tv_sec) * 1000000;
  compTime = compTime + (finish.tv_usec - start.tv_usec);
  Time = (double)compTime;

  printf("The Application time with %d threads: %f Secs\n", num_threads, Time/1000000.0);
  free(A);
  free(B);
  free(C);

  return 0;
}

