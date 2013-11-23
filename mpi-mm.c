#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG 13

int main(int argc, char *argv[]) {
  double **A, **B, **C, *tmp;
  double computationTime = 0.0, totalComputationTime = 0.0, checksum = 0.0, applicationTime = 0.0, startComputationTime = 0.0, endComputationTime = 0.0; 
  int numElements, offset, stripSize, myrank, numnodes, N, i, j, k;
  
  MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &numnodes);
  if (myrank == 0)
	applicationTime = MPI_Wtime();
  N = atoi(argv[1]);
  
  // allocate A, B, and C --- note that you want these to be
  // contiguously allocated.  Workers need less memory allocated.
  
  if (myrank == 0) {
    tmp = (double *) malloc (sizeof(double ) * N * N);
    A = (double **) malloc (sizeof(double *) * N);
    for (i = 0; i < N; i++)
      A[i] = &tmp[i * N];
  }
  else {
    tmp = (double *) malloc (sizeof(double ) * N * N / numnodes);
    A = (double **) malloc (sizeof(double *) * N / numnodes);
    for (i = 0; i < N / numnodes; i++)
      A[i] = &tmp[i * N];
  }
  
  
  tmp = (double *) malloc (sizeof(double ) * N * N);
  B = (double **) malloc (sizeof(double *) * N);
  for (i = 0; i < N; i++)
    B[i] = &tmp[i * N];
  
  
  if (myrank == 0) {
    tmp = (double *) malloc (sizeof(double ) * N * N);
    C = (double **) malloc (sizeof(double *) * N);
    for (i = 0; i < N; i++)
      C[i] = &tmp[i * N];
  }
  else {
    tmp = (double *) malloc (sizeof(double ) * N * N / numnodes);
    C = (double **) malloc (sizeof(double *) * N / numnodes);
    for (i = 0; i < N / numnodes; i++)
      C[i] = &tmp[i * N];
  }

  if (myrank == 0) {
    srand(1);
    // initialize A and B
    for (i=0; i<N; i++) {
      for (j=0; j<N; j++) {
        A[i][j] = 0.5 + (double)rand()/RAND_MAX;
        B[i][j] = 0.5 + (double)rand()/RAND_MAX;
      }
    }
  }
  
  stripSize = N/numnodes;

  // send each node its piece of A -- note could be done via MPI_Scatter
  if (myrank == 0) {
    offset = stripSize;
    numElements = stripSize * N;
    for (i=1; i<numnodes; i++) {
      MPI_Send(A[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
      offset += stripSize;
    }
  }
  else {  // receive my part of A   
    MPI_Recv(A[0], stripSize * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  
  // everyone gets B
  MPI_Bcast(B[0], N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Let each process initialize C to zero 
  for (i=0; i<stripSize; i++) {
    for (j=0; j<N; j++) {
      C[i][j] = 0.0;
    }
  }

  // do the work
  // start timer =========================================================================================================
  startComputationTime = MPI_Wtime();

  for (i=0; i<stripSize; i++)
    for (j=0; j<N; j++)
      for (k=0; k<N; k++)
	C[i][j] += A[i][k] * B[k][j];

  // stop timer =========================================================================================================
  endComputationTime = MPI_Wtime();
  computationTime += endComputationTime - startComputationTime;

  // master receives from workers  -- note could be done via MPI_Gather
  if (myrank == 0) {
    offset = stripSize; 
    numElements = stripSize * N;
    for (i=1; i<numnodes; i++) {
      MPI_Recv(C[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      offset += stripSize;
    }
  }
  else { // send my contribution to C
    MPI_Send(C[0], stripSize * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
  }
  
  MPI_Reduce(&computationTime, &totalComputationTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  
  
  // print out matrix here, if I'm the master
  if (myrank == 0) {
    for (i=0; i<N; i++) 
      for (j=0; j<N; j++) 
        checksum += C[i][j];
  }
  free(*A);
  free(A);
  free(*B);
  free(B);
  free(*C);
  free(C);
  if (myrank == 0)
  {
	applicationTime = MPI_Wtime() - applicationTime;
	printf("Checksum: %e, Application time: %f, Total time: %f\n",checksum, applicationTime, totalComputationTime);
  }
  MPI_Finalize();
  return 0;
}


