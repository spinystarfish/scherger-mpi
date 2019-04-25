#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/** Global Variables **/

struct Send_Inst {
  int sender;
  int receiver;
  char msg[256];
};

/** Function Definitions **/

int Read_Exec();

void Report_End(int rank, int clock);

void Report_Exec(int rank, int clock);

void Report_Rec(int rank, int sendrank, char* msg, int clock);

void Report_Send(int rank, int receiverank, char* msg, int clock);


/***********************/

int main(int argc, char* argv[]){
  /*Local Variables */

  int rank;
  int size;
  MPI_Status status;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  /* THE MANAGER PROCESS */
  if(rank == 0) {
    scanf("%d", &size);
    printf("Number of processes: %d\n", size);
    
    for(int i = 0; i < 2; i++) {
      char tmp[128];
      scanf("%s", &tmp);
      printf("%s\n", tmp);
    }
    /*
    for(int i = 0; i < 1; i++) {
      char* event;
      int d = scanf("%s", &event);
      printf("%d: %s.\n", d, event);
    }
    */
    /** In child processes, while loop. MSG RECV - if die tag exit, otherwise do stuff **/
  }
  
  MPI_Finalize();
}

int Read_Exec() {
  int d;
  scanf("%d", &d);
  return d;
}

void Report_End(int rank, int clock) {
  printf("\t[%d]: Logical Clock = %d\n", rank, clock);
}

void Report_Exec(int rank, int clock) {
  printf("\t[%d]: Execution Event: Logical Clock = %d\n", rank, clock);
}

void Report_Rec(int rank, int sendrank, char* msg, int clock) {
  printf("\t[%d]: Message Received from %d: Message >%s<: Logical Clock = %d\n", rank, sendrank, msg, clock);
}

void Report_Send(int rank, int receiverank, char* msg, int clock) {
  printf("\t[%d]: Message Send to %d: Message >%s<: Logical Clock = %d\n", rank, receiverank, msg, clock);
}
