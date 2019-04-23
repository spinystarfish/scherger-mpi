#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/** Global Variables **/

/** Function Definitions **/

void Report_End(int rank, int clock);

void Report_Exec(int rank, int clock);

void Report_Rec(int rank, int sendrank, char* msg, int clock);

void Report_Send(int rank, int receiverank, char* msg, int clock);


/***********************/

int main(int argc, char* argv[]){
  int size;
  char* event[10];
  /* THE MANAGER PROCESS */
  /* Can use this for debugging/validating purposes*/
  scanf("%d", &size);
  printf("[0]: There are %d processes in the system\n", size);

  while(scanf("%s", &event) == 1) {
    printf("An event has occured!: %s\n", event);
  }


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
