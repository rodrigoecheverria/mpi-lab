#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

#define TAG 13
void logIfMaster(int rank, const char * format, ...);
int main(int argc, char *argv[]) {
  double *Ag, **A, *b, *bg, *x1, *x2, *last, *current, *tmp;
  double computationTime = 0.0, totalComputationTime = 0.0, checksum = 0.0, applicationTime = 0.0, startComputationTime = 0.0, endComputationTime = 0.0, sum_a, max_diff, local_max_diff, diff, epsilon; 
  int numElements, offset, stripSize, myrank, numnodes, c, N, i, j, k, it_count;
  MPI_Datatype strip;

  MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &numnodes);

  if (myrank == 0)
	  applicationTime = MPI_Wtime();
  c = atoi(argv[1]);
  N = (c>0) ? c * 1024 : 2;
  epsilon = pow(10, -(atoi(argv[2])));
  it_count = 0;
  
  /*//Logging
  char logFileName[16];
  snprintf(logFileName, 16, "log_%d.txt",myrank); 
  FILE *f = fopen(logFileName,"w");
  if (f == NULL)
  {
    printf("Could not open file for process: %d\n",myrank);
    exit(1);
  }*/
//Strip calculation ===================================================================
  stripSize = N/numnodes;
  MPI_Type_vector(stripSize, N, N, MPI_DOUBLE, &strip);
  MPI_Type_commit(&strip); 
    
//Memory allocation ===================================================================
  if (myrank == 0) {
    Ag = (double *) malloc (sizeof(double) * N * N);
    bg = (double *) malloc (sizeof(double) * N);
  }
  tmp = (double *)malloc(sizeof(double) * stripSize * N);
  A = (double **) malloc (sizeof(double *) * stripSize);
  b = (double *) malloc (sizeof(double) * stripSize);
  x1 = (double *) malloc (sizeof(double) * N);
  x2 = (double *) malloc (sizeof(double) * N);
  for (i = 0; i < stripSize; i++)
    A[i] = &(tmp[i * N]);
  
  
//Initialize matrixes  =================================================================
  if (myrank == 0) {
   if (c > 0) {
    srand(1);
    // initialize Ag, bg & x to random values
    for (i=0; i<N; i++) {
      for (j=0; j<N; j++) {
        Ag[i*N + j] = 0.5 + (double)rand()/RAND_MAX;
      }
      bg[i] = 0.5 + (double)rand()/RAND_MAX;
      x1[i] = bg[i];
    }
    
    //Enforce diagonal dominancy in Ag
    for (i=0; i<N; i++) {
      Ag[i*N + j] = 0.0;
      for (j=0; j<N; j++) {
        Ag[i*N + i]+=Ag[i*N + j];
      }
    } 
  } else { // Test case
      Ag[0] = 2.0;  Ag[1] = 1.0; Ag[2] = 5.0;  Ag[3] = 7.0;
      //Ag[4] = 1.0;  Ag[5] = -4.0; Ag[6] = 2.0;  Ag[7] = 1.0;
      //Ag[8] = -1.0; Ag[9] = 2.0;  Ag[10] = 4.0; Ag[11] = 1.0;
      //Ag[12] = 1.0; Ag[13] = 2.0; Ag[14] = 1.0; Ag[15] = -4.0;
      
      bg[0] = 11.0;  bg[1] = 13.0;  //bg[2] = 2.0;  bg[3] = 2.0;
      
      x1[0] = 1;// bg[0]; 
      x1[1] = 1; //bg[1]; x1[2] = bg[2]; x1[3] = bg[3];
    }
  }
       
//Send initial data  ===================================================================
  MPI_Scatter(Ag, 1, strip, &(A[0][0]), 1, strip, 0, MPI_COMM_WORLD);
  MPI_Scatter(bg, stripSize, MPI_DOUBLE, &(b[0]), stripSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(x1,N,MPI_DOUBLE,0,MPI_COMM_WORLD);
  
//Jacobi iterations  ===================================================================
  //ping-pong buffer between x1 and x2 initialition
  last = x1;
  current = x2;

  do {
    //Main computation
    offset = stripSize * myrank;
    for (i=0; i<stripSize; i++) {
      sum_a = 0.0;
      for (j=0; j < offset + i; j++){
        sum_a += A[i][j]*last[j];
      }
      for (j=offset+i+1; j<N; j++){
        sum_a += A[i][j]*last[j]; 
      }
      current[i] = (b[i] - sum_a)/A[i][offset + i];
    }
    //End of main computation
    
    //Calculate my local max diff
    local_max_diff = abs(current[0] - last[0]);
    diff = 0.0;
    for (i=1; i<stripSize; i++) {
      diff = abs(current[i] - last[i]);
      if (diff > local_max_diff) local_max_diff = diff;
    }
    
    //interchange x-buffers
    tmp = last;
    last = current;
    current = tmp;
    
    //calculate convergence : max (local_max_errors) > epsilon
    MPI_Allreduce(&local_max_diff, &max_diff, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    printf ("Iteration: %d\n",it_count++);
  } while (max_diff > epsilon);
  MPI_Barrier(MPI_COMM_WORLD);
  if (myrank == 0)
    for (i = 0; i < N; i++)
      printf("x1[%d]=%f x2[%d]=%f \n",i,x1[i],i,x2[i]);
  /*free(*A);
  free(A);
  free(b);
  free(bg);
  free(Ag);
  free(x1);
  //close log*/
  MPI_Finalize();
  return 0;

}

void logIfMaster(int rank, const char * format, ...)
{
  va_list arglist;
  va_start(arglist,format);
  if (rank == 0) vprintf(format, arglist);
}


