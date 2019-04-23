#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){

  // local variables
  int rank;
  int size;
  int i;
  int data[5];
  MPI_Status status;
  

  
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(rank == 0){
    i = 1;
    printf("[%d] sent %d to %d\n", rank, i, 1);
    MPI_Send(&i, 1, MPI_INT, 1, 7, MPI_COMM_WORLD);
    MPI_Recv(&i, 1, MPI_INT, size-1, 7, MPI_COMM_WORLD, &status);
    printf("[%d] received %d from %d\n", rank, i, size-1);

  }
  else{
    MPI_Recv(&i, 1, MPI_INT, rank-1, 7, MPI_COMM_WORLD, &status);
    i = i*rank;
    printf("[%d] sent %d to %d\n", rank, i, (rank+1)%size);
    MPI_Send(&i, 1, MPI_INT, (rank+1)%size, 7, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return(0);
}
