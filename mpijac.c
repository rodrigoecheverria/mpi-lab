#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG 13
int main(int argc, char *argv[]) {
  double **Ag, **A, *b, *bg, *x, *tmp, *tmp1;
  double computationTime = 0.0, totalComputationTime = 0.0, checksum = 0.0, applicationTime = 0.0, startComputationTime = 0.0, endComputationTime = 0.0; 
  int numElements, offset, stripSize, myrank, numnodes, c, N, i, j, k;
  MPI_Datatype strip;

  MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &numnodes);

  if (myrank == 0)
	  applicationTime = MPI_Wtime();
  c = atoi(argv[1]);
  N = (c>0) ? c * 1024 : 3;

//Strip stuff
  stripSize = N/numnodes;
  MPI_Type_vector(stripSize, N, N, MPI_DOUBLE, &strip);
  MPI_Type_commit(&strip); 

//Memory allocation
  if (myrank == 0) {
    tmp1 = (double *) malloc (sizeof(double ) * N * N);
    Ag = (double **) malloc (sizeof(double *) * N);
    bg = (double *) malloc (sizeof(double) * N);
    for (i = 0; i < N; i++)
      Ag[i] = &tmp1[i * N];
  }
  tmp = (double *) malloc (sizeof(double ) * N * stripSize);
  A = (double **) malloc (sizeof(double *) * stripSize);
  b = (double *) malloc (sizeof(double) * stripSize);
  x = (double *) malloc (sizeof(double) * N);
  for (i = 0; i < stripSize; i++)
    A[i] = &tmp[i * N];
  
  
//Initialize matrixes
  if ((myrank ==0)&&(c > 0)) {
    srand(1);
    // initialize A and B
    for (i=0; i<N; i++) {
      for (j=0; j<N; j++) {
        Ag[i][j] = 0.5 + (double)rand()/RAND_MAX;
      }
      bg[i] = 0.5 + (double)rand()/RAND_MAX;
      x[i] = bg[i];
    }
    for (i=0; i<N; i++) {
      Ag[i][i] = 0.0;
      for (j=0; j<N; j++) {
        Ag[i][i]+=Ag[i][j];
      }
    } 
  }
  if ((myrank == 0)&&(c == 0)) {
    Ag[0][0] = 3.0;
    Ag[0][1] = -2.0;
    Ag[0][2] = 1.0;
    Ag[1][0] = 1.0;
    Ag[1][1] = -3.0;
    Ag[1][2] = 2.0;
    Ag[2][0] = -1.0;
    Ag[2][1] = 2.0;
    Ag[2][2] = 4.0;
    bg[0] = 1.0;
    bg[1] = 1.0;
    bg[2] = 1.0;
    x[0] = b[0];
    x[1] = b[1];
    x[2] = b[2];
  }
    printf("hasta aqui1\n");
//Send data
  MPI_Scatter(tmp1, 1, strip, &(A[0][0]), 1, strip, 0, MPI_COMM_WORLD);
    printf("hasta aqui\n");

  if (myrank == 0)
  {
    for (i=0; i<N; i++) {
      for (j=0; j<N; j++)
         printf ("%f ",Ag[i][j]);
      printf("\n");
     }
  }
  
  for (i=0; i<stripSize; i++) {
      for (j=0; j<N; j++)
         printf ("%f ",A[i][j]);
      printf("\n");
     }

  free(*A);
  free(A);
  free(b);
  free(bg);
  free(*Ag);
  free(Ag);
  free(x);
  MPI_Finalize();
  return 0;

}
