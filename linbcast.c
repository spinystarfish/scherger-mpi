#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){

  // local variables
  int rank;
  int size;
  int i;
  int j;
  int sum=0;
  int data[5];
  MPI_Status status;
  

  
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(rank == 0){
    for(i=1; i<size; i++){
      MPI_Send(&i, 1, MPI_INT, i, 7, MPI_COMM_WORLD);
      printf("[%d] sent %d to %d\n", rank, i, i);
    }

    for(i=1; i<size; i++){
      MPI_Recv(&j, 1, MPI_INT, MPI_ANY_SOURCE, 7, MPI_COMM_WORLD, &status);
      printf("[%d] recv %d from %d\n", rank, j, status.MPI_SOURCE);
      sum += j;
    }
    printf("[%d] sum = %d\n", 0, sum);
  }
  else{
    MPI_Recv(&i, 1, MPI_INT, 0, 7, MPI_COMM_WORLD, &status);
    i = i+rank;
    printf("[%d] recv and modified %d from %d\n", rank, i, 0);
    MPI_Send(&i, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return(0);
}
