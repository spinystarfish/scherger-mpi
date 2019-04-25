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

void Report_End(int rank, int clock);

void Report_Exec(int rank, int clock);

void Report_Rec(int rank, int sendrank, char* msg, int clock);

void Report_Send(int rank, int receiverank, char* msg, int clock);


/***********************/

int main(int argc, char* argv[]){
  /*Local Variables */
  
  int size;
  char* event[10];
  int sendID;
  int recieveID;
  char* msg;
  /* THE MANAGER PROCESS */
  /* Can use this for debugging/validating purposes*/
  scanf("%d", &size);
  printf("[0]: There are %d processes in the system\n", size);

  /*For each line (a.k.a each event)*/
  while(scanf("%s", &event) == 1) {
    /* Parse Method */


    scanf("%d", &sendID);
    scanf("%d", &recieveID);
    scanf(" %[^\n]", msg);
    printf("An event has occured!: %s %d %d %s\n", event, sendID, recieveID, msg);
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
