#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

/** Global Variables **/

struct Event {
  int type; //0 => exec; 1 => send; 2 => end; 3 => error
  int sender;
  int receiver;
  char msg[256];
};

/** Function Definitions **/

struct Event Read_Event();

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

    struct Event e = Read_Event();
    while(e.type == 0 || e.type == 1) {
      if(e.type == 0) {
	printf("\tExec: %d\n", e.sender);
      }
      else {
	printf("\tSend: %d %d %s\n", e.sender, e.receiver, e.msg);
      }
      e = Read_Event();
    }
    
    
    /** In child processes, while loop. MSG RECV - if die tag exit, otherwise do stuff **/
  }
  
  MPI_Finalize();
}

struct Event Read_Event() {
  struct Event e;
  e.sender = -1;
  e.receiver = -1;
  
  char ev[10];
  scanf("%s", &ev); //read event type
  
  if( !strcmp(ev, "exec") ) {
    e.type = 0;
    scanf("%d", &e.sender);
    return e;
  }
  else if( !strcmp(ev, "send") ){
    e.type = 1;
    scanf("%d", &e.sender);
    scanf("%d", &e.receiver);
    scanf("%[^\n]", &e.msg);
    return e;
    //TODO Read the message to be sent
  }
  else if( !strcmp(ev, "end") ) {
    e.type = 2;
    return e;
  }
  else {
    e.type = 3;
    printf("Event type not recognized.\n");
    return e;
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
