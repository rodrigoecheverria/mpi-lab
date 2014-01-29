#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG 13

void PrintMatrix(double **Mat, int N, int M);

int main(int argc, char *argv[]) {
  double **A, **B, **C, *tmp;
  double  applicationTime = 0.0, computationTime = 0.0, startGlobalComputationTime = 0.0, endGlobalComputationTime = 0.0,
			 startLocalComputationTime = 0.0, endLocalComputationTime = 0.0, checksum = 0.0; 
  int numElements, offset, stripSize, myrank, numnodes, N, M, i, j, k;
  
// Initialization =================================================================== 
  MPI_Init(&argc, &argv);
 
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &numnodes);

  if (myrank == 0) applicationTime = MPI_Wtime();
    
  N = atoi(argv[1]);
  M = atoi(argv[2]);
  
  stripSize = M / numnodes; //Quantity of work done per node (divide A row-wise)
  
// Memory allocation ================================================================
//   allocate A, B, and C --- note that you want these to be
//   contiguously allocated.  Workers need less memory allocated.
// ==================================================================================
// A:
  if (myrank == 0) {
    tmp = (double *) malloc (sizeof(double ) * N * M);
    A = (double **) malloc (sizeof(double *) * M);
    for (i = 0; i < M; i++)
      A[i] = &tmp[i * N];
  } else {
    tmp = (double *) malloc (sizeof(double ) * N * stripSize);
    A = (double **) malloc (sizeof(double *) * stripSize);
    for (i = 0; i < stripSize; i++)
      A[i] = &tmp[i * N];
  }
  
// B:
  tmp = (double *) malloc (sizeof(double ) * N * M);
  B = (double **) malloc (sizeof(double *) * M);
  for (i = 0; i < M; i++)
    B[i] = &tmp[i * N];
  
// C:
  if (myrank == 0) {
    tmp = (double *) malloc (sizeof(double ) * M * M);
    C = (double **) malloc (sizeof(double *) * M);
    for (i = 0; i < M; i++)
      C[i] = &tmp[i * M];
  } else {
    tmp = (double *) malloc (sizeof(double ) * M * stripSize);
    C = (double **) malloc (sizeof(double *) * stripSize);
    for (i = 0; i < stripSize; i++)
      C[i] = &tmp[i * M];
  }

// Matrix initialization (Random) ===================================================
  if (myrank == 0) {
    srand(1);
    for (i = 0; i < M; i++) 
      for (j = 0; j < N; j++) {
        A[i][j] = 0.5 + (double)rand()/RAND_MAX;
        B[i][j] = 0.5 + (double)rand()/RAND_MAX;
      }
  }
  
// Send initial data ================================================================
// A:
  if (myrank == 0) {
    offset = stripSize;
    numElements = stripSize * N;
    for (i = 1; i < numnodes; i++) {
      MPI_Send(A[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
      offset += stripSize;
    }
  } else {
    MPI_Recv(A[0], stripSize * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  
// B:
  MPI_Bcast(B[0], N * M, MPI_DOUBLE, 0, MPI_COMM_WORLD);


// Computation ======================================================================
  starLocalComputationTime = MPI_Wtime();
 
// Initialize C to 0
  for (i = 0; i < stripSize; i++)
    for (j = 0; j < N; j++)
      C[i][j] = 0.0;

// Compute my C elements
  for (i = 0; i < stripSize; i++)
    for (j = 0; j < M; j++)
      for (k = 0; k < N; k++)
	      C[i][j] += A[i][k] * B[j][k]; //B is transposed

// Stop timer & reduce start and end
  endLocalComputationTime = MPI_Wtime();
  MPI_Reduce(&starLocalComputationTime, &starGlobalComputationTime, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
  MPI_Reduce(&endLocalComputationTime, &endGlobalComputationTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (myrank == 0) computationTime = endGlobalComputationTime - starGlobalComputationTime;
// Computation End ==================================================================

//Receive Computed Data =============================================================
  if (myrank == 0) {
    offset = stripSize; 
    numElements = stripSize * M;
    for (i = 1; i < numnodes; i++) {
      MPI_Recv(C[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, 
                MPI_STATUS_IGNORE);
      offset += stripSize;
    }
  }
  else { 
    MPI_Send(C[0], stripSize * M, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
  }
  
// Calculate the checksum ===========================================================
  if (myrank == 0) {
    for (i=0; i<M; i++) 
      for (j=0; j<M; j++)
        checksum += C[i][j];
  }
  
  if (myrank == 0) {
	  applicationTime = MPI_Wtime() - applicationTime;
    //PrintMatrix(C,M,M);
	  printf("Checksum: %f, Application time: %f, Total computation time: %f\n", checksum, 
	          applicationTime, computationTime);
  }
    
  free(*A);
  free(A);
  free(*B);
  free(B);
  free(*C);
  free(C);
  MPI_Finalize();
  return 0;
}

void PrintMatrix(double **Mat, int N, int M)
{
  if (N < 20 && M < 20) {
    int i,j;
    for (i=0; i<N; i++){
      for (j=0; j<M; j++)
        printf("%f,",Mat[i][j]);
      printf("\n");
    }
  }
}

