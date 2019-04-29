#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

/* TAGS, all of which are totally arbitrary because I came up with them on the spot
 * 40 => Single integer specifying length of char message to follow. If -1, end
 * 55 => Character message of length specified by preceding message.
 * 70 => Single integer specifying the ending clock of a process.
 */

/** Global Variables **/

struct Event {
  int type; 
  /*0 => exec
   * 1 => send
   * 2 => end
   * 3 => error
   */
  int sender;
  int receiver;
  char msg[256];
};

/** Function Definitions **/

struct Event Deserialize_Event(char* serial, int len);

void Serialize_Event(struct Event e, char* serial, int* len);

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


  if(rank == 0) { //Manager Process
    int sim_size;
    scanf("%d", &sim_size);
    fprintf(stdout, "[0]: There are %d processes in the system\n", size);

    //Reads in events and sends events to appropriate processes
    struct Event e = Read_Event();
    while(e.type == 0 || e.type == 1) {
      char serial[262];
      int slen;
      Serialize_Event(e, serial, &slen);
      //fprintf(stdout, "Sending Out %d: %s\n", slen, serial);
      
      //KENNY: We don't need to do this, its optional. I can explain. 
      MPI_Send(&slen, 1, MPI_INT, e.sender, 40, MPI_COMM_WORLD);//send length
      
      MPI_Send(&serial, slen, MPI_CHAR, e.sender, 55, MPI_COMM_WORLD);//send event
      /*
      //KENNY: I'd rather have the simluation processes do the printing	
      if(e.type == 0) {
        //fprintf(stdout, "\tExec: %d\n", e.sender);
        MPI_Send(&e.type, 1, MPI_INT, e.sender, 55, MPI_COMM_WORLD);
      }
      else if(e.type == 1) {
        fprintf(stdout, "\tNOT Send: %d %d %s\n", e.sender, e.receiver, e.msg);
      }
      */
      e = Read_Event();
    }
    //SIMULATION ENDING
    //print out logical clock values
    for(int p = 1; p < size; p++) {
      int end = -1;
      MPI_Send(&end, 1, MPI_INT, p, 40, MPI_COMM_WORLD);
      int lclock;
      MPI_Recv(&lclock, 1, MPI_INT, p, 70, MPI_COMM_WORLD, &status);
      if(p == 1) {
        fprintf(stdout, "[0]: Simulation ending\n");
      }
      Report_End(p, lclock);
    }
    
    //Simulation proccess
  } else {
    int clock = 0;
    //While Simulation is Running
    while(1) {
      //KENNY: Also part of the optional, not neccesary mention I made above
      int ilen;
      MPI_Recv(&ilen, 1, MPI_INT, MPI_ANY_SOURCE, 40, MPI_COMM_WORLD, &status);
      if(ilen == -1) { //end
	//Quit the Simulation
        break;
      }
      //Recieving Message...
      char input[ilen];
      MPI_Recv(&input, ilen, MPI_CHAR, MPI_ANY_SOURCE, 55, MPI_COMM_WORLD, &status);
      //fprintf(stdout, "%d To Deserial %d: %s\n", rank, ilen, input);
      struct Event e = Deserialize_Event(input, ilen);
      //Exec Intruction Recieved
      if(e.type == 0) {
        Report_Exec(rank, ++clock);
      }
      //Send Instruction Recieved
      else if(e.type == 1) {
        //TODO send message to another process
	//Recieved from Manager Process
	//if(e.sender == rank) {
	//  Report_Send(rank, e.reciever , e.msg, clock++);
	//}
	//Recieved from Simulation Process
        //else {
	//  Report_Rec(rank, e.sender, e.msg, clock++);
	//}
      }
      //End Instruction Recieved -> Error
      else if(e.type == 2) {
        fprintf(stdout, "How did we get here?\n");
        break;
      }
    }
    //Send the Finish Clock Time to Manager
    MPI_Send(&clock, 1, MPI_INT, 0, 70, MPI_COMM_WORLD);
  }
  
  MPI_Finalize();
}

/*
 * Deserialize_Event
 * Takes a serialized event and its length, returns
 * the deserialized Event struct. 
 */
struct Event Deserialize_Event(char* serial, int len) {
  struct Event e;
  e.type = atoi(serial);
  if(e.type == 0) {//EXEC "t|s"
    e.sender = atoi(serial+2);
    e.receiver = -1;
  }
  else if(e.type == 1) {//SEND "t|s|r|m"
    e.sender = atoi(serial+2);
    e.receiver = atoi(serial+4);
    sprintf(e.msg, "%s\0", serial+6);
  }
  
  return e;
}

/*
 * Serialize_Event
 * Given an Event struct, serialize it into a char*
 * (serial) with discovered length (len). Serial
 * must have memory allocated already [262].
 */
void Serialize_Event(struct Event e, char* serial, int* len) {
  sprintf(serial, "%d|", e.type);
  if(e.type == 0) {//EXEC "t|s"
    sprintf(serial+2, "%d\0", e.sender);
    *len = 4;
    return;
  }
  else if(e.type == 1) {//SEND "t|s|r|m"
    sprintf(serial+2, "%d|", e.sender);
    sprintf(serial+4, "%d|", e.receiver);
    sprintf(serial+6, "%s\0", e.msg);
    *len = 7 + strlen(e.msg);
  }
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
    /* "Sender" in this case really means the process that
     *  will be simulating an instruction execution.
     */
    scanf("%d", &e.sender);
    return e;
  }
  else if( !strcmp(etype, "send") ){//if type == send
    e.type = 1;
    scanf("%d", &e.sender);
    scanf("%d", &e.receiver);
    char tmp;
    scanf("%c", &tmp);//skip a space
    scanf("%c", &tmp);//skip starting "
    scanf("%[^\"]", &e.msg);
    scanf("%c", &tmp);//skip ending "
    return e;
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
