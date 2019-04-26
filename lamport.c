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


  if(rank == 0) {//Manager Process
    int sim_size;
    scanf("%d", &sim_size);
    fprintf(stdout, "[0]: There are %d processes in the system\n", size);

    //Currently, simply reads in events and echos to stdout
    struct Event e = Read_Event();
    while(e.type == 0 || e.type == 1) {
      if(e.type == 0) {
        //fprintf(stdout, "\tExec: %d\n", e.sender);
      }
      else {
        fprintf(stdout, "\tNOT Send: %d %d %s\n", e.sender, e.receiver, e.msg);
      }
      e = Read_Event();
    }

    fprintf(stdout, "[0]: Simulation ending\n");
    
  } else {
    /** In child processes, while loop. MSG RECV - if die tag exit, otherwise do stuff **/
    
  }
  
  MPI_Finalize();
}

/*
 * Read_Event
 * Takes the next line of stdin and saves the details in
 * an event struct. If a part of the event struct is not 
 * needed, then it is either initialized to -1 or left null.
 */
struct Event Read_Event() {
  struct Event e;
  e.sender = -1;
  e.receiver = -1;
  
  char etype[10];
  scanf("%s", &etype); //read event type
  
  if( !strcmp(etype, "exec") ) {//if type == exec
    e.type = 0;
    /* Sender is an imporper name here. It is simply the process
     * which will be simulation an instruction execution.
     */
    scanf("%d", &e.sender);
    return e;
  }
  else if( !strcmp(etype, "send") ){//if type == send
    e.type = 1;
    scanf("%d", &e.sender);
    scanf("%d", &e.receiver);
    scanf("%[^\n]", &e.msg);
    return e;
    //TODO Read the message to be sent
  }
  else if( !strcmp(etype, "end") ) {//if type == end
    e.type = 2;
    return e;
  }
  else {//if type not recognized
    /* This should never be triggered given proper input.
     * If input is fine, we have a problem elsewhere.
     */
    e.type = 3;
    fprintf(stderr, "Event type not recognized.\n");
    return e;
  }
}

/*
 * Report_End
 * A standard print of final logical clock status
 * for a particular process. 
 */
void Report_End(int rank, int clock) {
  fprintf(stdout, "\t[%d]: Logical Clock = %d\n", rank, clock);
}

/*
 * Report_Exec
 * A standard print of logical clock status upon
 * simulating an instruction execution.
 */
void Report_Exec(int rank, int clock) {
  fprintf(stdout, "\t[%d]: Execution Event: Logical Clock = %d\n", rank, clock);
}

/*
 * Report_Rec
 * A standard print of logical clock status upon
 * simulating a message receive.
 */
void Report_Rec(int rank, int sendrank, char* msg, int clock) {
  fprintf(stdout, "\t[%d]: Message Received from %d: Message >%s<: Logical Clock = %d\n", rank, sendrank, msg, clock);
}

/*
 * Report_Send
 * A standard print of logical clock status upon
 * simulating a message send.
 */
void Report_Send(int rank, int receiverank, char* msg, int clock) {
  fprintf(stdout, "\t[%d]: Message Send to %d: Message >%s<: Logical Clock = %d\n", rank, receiverank, msg, clock);
}
