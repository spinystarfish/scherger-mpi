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

  printf("Hello from MPI. I am %d of %d\n", rank, size);

  if(rank == 0){
    for(i=0; i<5; i++)
      data[i] = (i+1)*10;
    MPI_Send(data, 5, MPI_INT, 1, 555, MPI_COMM_WORLD);
  }
  else if(rank == 1){
    MPI_Recv(data, 5, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    for(i=0; i<5; i++)
      printf("%d ",data[i]);
    printf("\n");    
  }

  MPI_Finalize();

  return(0);
}
