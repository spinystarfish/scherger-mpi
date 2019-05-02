#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* TAGS, all of which are totally arbitrary because I came up with them on the spot
 * 30 => Array of clocks integers
 * 55 => Character message of length specified by preceding message.
 * 60 => Messages between simulation processes
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

struct Event Deserialize_Event(char* serial);

int Digits(int i);

void Manager(int size);

int Max(int a, int b);

void Print_Clocks(int* clocks, int len, int rank, char* name);

struct Event Read_Event();

void Report_End(int rank, int clock);

void Report_Exec(int rank, int clock);

void Report_Rec(int rank, int sendrank, char* msg, int clock);

void Report_Send(int rank, int receiverank, char* msg, int clock);

void Serialize_Event(struct Event e, char* serial, int* len);

void Sim_Process(int size, int rank);

void Update_Clocks(int* tmpclocks, int* clocks, int len);


int main(int argc, char* argv[]){
  /*Local Variables */

  int rank;
  int size;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  if(rank == 0) { //Manager Process
    Manager(size);
  } else { //Simulation Processes
    Sim_Process(size, rank);
  }
  
  MPI_Finalize();
}

/*
 * Deserialize_Event
 * Takes a serialized event and its length, returns
 * the deserialized Event struct. 
 */
struct Event Deserialize_Event(char* serial) {
  struct Event e;
  e.type = atoi(serial);
  int index = Digits(e.type) + 1;
  if(e.type == 0) {//EXEC "type|sender"
    e.sender = atoi(serial+index);
    e.receiver = -1;
  }
  else if(e.type == 1) {//SEND "type|sender|receiver|message"
    e.sender = atoi(serial+index);
    index += Digits(e.sender) + 1;
    e.receiver = atoi(serial+index);
    index += Digits(e.receiver) + 1;
    sprintf(e.msg, "%s\0", serial+index);
  }
  
  return e;
}

/*
 * Digits
 * Returns the number of chars which the number would take up if 
 * converted to a char array.
 */
int Digits(int i) {
  char temp[10];
  sprintf(temp, "%d", i);
  return strlen(temp);
}

/*
 * Manager
 * Carries out the tasks of the Manager Process
 */
void Manager(int size) {
  int sim_size;
  scanf("%d", &sim_size);
  fprintf(stdout, "[0]: There are %d processes in the system\n", sim_size);

  //Reads in events and sends events to appropriate processes
  struct Event e = Read_Event();
  while(e.type == 0 || e.type == 1) {
    char serial[300];
    int slen;
    Serialize_Event(e, serial, &slen);
    //fprintf(stdout, "Sending Out %d: %s\n", slen, serial);
    if(e.type == 1) {
      //if send event, send a warning to receiver to prepare
      MPI_Send(&serial, slen, MPI_CHAR, e.receiver, 55, MPI_COMM_WORLD);//send event
    }
    MPI_Send(&serial, slen, MPI_CHAR, e.sender, 55, MPI_COMM_WORLD);//send event
    e = Read_Event();
  }
  
  //SIMULATION ENDING
  //print out logical clock values
  sleep(1);
  for(int p = 1; p < size; p++) {
    char end_serial[4] = "end\0";
    MPI_Send(&end_serial, 4, MPI_CHAR, p, 55, MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  /* TODO: Print out vectors, not individual clock values
  int end_clocks[size];
  fprintf(stdout, "[0]: Simulation ending\n");
  for(int p = 1; p < size; p++) {
    int lclock;
    MPI_Status status;
    MPI_Recv(&lclock, 1, MPI_INT, p, 70, MPI_COMM_WORLD, &status);
    Report_End(p, lclock);
  
  }
  */
}

/*
 * Max
 * Returns the larger of the two integers.
 */
int Max(int a, int b) {
  if( a > b )
    return a;
  else
    return b;
}

/*
 * Print_Clocks
 * Simply a statement used for debugging.
 * Should not be used in the final version 
 * and can be deleted.
 */
void Print_Clocks(int* clocks, int len, int rank, char* name) {
  fprintf(stdout, "%d %s: ", rank, name);
  fflush(stdout);
  for(int i = 0; i < len; i++ )
    fprintf(stdout, "%d ", clocks[i]);
  fprintf(stdout, "\n");
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

/*
 * Serialize_Event
 * Given an Event struct, serialize it into a char*
 * (serial) with discovered length (len). Serial
 * must have memory allocated already [300].
 */
void Serialize_Event(struct Event e, char* serial, int* len) {
  sprintf(serial, "%d|", e.type);
  int index = 2;
  if(e.type == 0) {//EXEC "type|sender"
    sprintf(serial+index, "%d\0", e.sender);
    index += Digits(e.sender) + 1;
  }
  else if(e.type == 1) {//SEND "type|sender|receiver|message"
    sprintf(serial+index, "%d|", e.sender);
    index += Digits(e.sender) + 1;
    sprintf(serial+index, "%d|", e.receiver);
    index += Digits(e.receiver) + 1;
    sprintf(serial+index, "%s\0", e.msg);
    index += strlen(e.msg) + 1;
  }
  *len = index;
}

/*
 * Sim_Process
 * Carries out the tasks of a simulation process.
 */
void Sim_Process(int size, int rank) {
  MPI_Status status;
  int* clocks = malloc((size-1) * sizeof(int));
  for(int i = 0; i < size-1; i++) {
    clocks[i] = 0;
  }
  //While Simulation is Running
  while(1) {
    //Recieving Message...
    char input[300];
    MPI_Recv(&input, 300, MPI_CHAR, MPI_ANY_SOURCE, 55, MPI_COMM_WORLD, &status);
    if( !strcmp(input, "end") ) { //end
      //Quit the Simulation
      break;
    }
    //fprintf(stdout, "%d To Deserial %d: %s\n", rank, ilen, input);
    struct Event e = Deserialize_Event(input);
    
    if(e.type == 0) {
      //Exec Intruction Recieved
      Report_Exec(rank, ++clocks[rank-1]);
    }
    else if(e.type == 1) {//SEND
      if(rank == e.sender) {
        //if current process is the sender (received message from manager)
        Report_Send(rank, e.receiver, e.msg, ++clocks[rank-1]);
        
        //add clock to event and reserialize:
        char serial[300];
        int slen;
        Serialize_Event(e, serial, &slen);

        //pass your clocks
        MPI_Send(&clocks, size-1, MPI_INT, e.receiver, 30, MPI_COMM_WORLD);
        //pass message:
        MPI_Send(&serial, slen, MPI_CHAR, e.receiver, 60, MPI_COMM_WORLD);//send event
      } 
      else {
        //if current process is the receiver (received message from sender)
  
        int* tmpclocks;// = malloc((size-1) * sizeof(int));
        MPI_Recv(&tmpclocks, size-1, MPI_INT, e.sender, 30, MPI_COMM_WORLD, &status);
        clocks[rank-1]++;
        //TODO: narrowed the problem down to tmpclocks not existing and always given a seg fault
        Print_Clocks(clocks, size-1, rank, "clocks");
        Print_Clocks(tmpclocks, size-1, rank, "tmpclocks");
        Update_Clocks(tmpclocks, clocks, size-1);
        fprintf(stdout, "%d has gotten here\n", rank);
        //TODO: Free tmpclocks
  
        //Wait for message from actual sender:
        MPI_Recv(&input, 300, MPI_CHAR, e.sender, 60, MPI_COMM_WORLD, &status);
        e = Deserialize_Event(input);
        
        Report_Rec(rank, e.sender, e.msg, ++clocks[rank-1]);
      }
    }
    else if(e.type == 2) {//END
      fprintf(stderr, "END message found in wrong location.\n");
      break;
    }
  }
  //Send the Finish Clock Time to Manager
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Send(&clocks, size-1, MPI_INT, 0, 30, MPI_COMM_WORLD);
}

/*
 * Update_Clocks
 * Given an array of integer clock values (tmpclocks),
 * find the max integer at each array location and place
 * it in the clocks array
 */
void Update_Clocks(int *tmpclocks, int *clocks, int len){
  for(int i = 0; i < len; i++)
    clocks[i] = Max(tmpclocks[i], clocks[i]);
}