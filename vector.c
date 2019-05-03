#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* TAGS, all of which are totally arbitrary because I came up with them on the spot
 * 55 => Messages sent from the Manager to a simulation process.
 * 60 => Messages between simulation processes.
 * 70 => Single integer specifying the ending clock of a process. Sim -> Manager
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
void Print_Clocks(int size, int* clocks);
int Max(int a, int b);
int Digits(int i);
int* Create_Clocks(int size);
int* Update_Clocks(int* tmpclocks, int* clocks, int size);
void Serialize_Event(struct Event e, char* serial, int* len);
struct Event Deserialize_Event(char* serial);
struct Event Read_Event();
void Report_End(int rank, int* clocks, int size);
void Report_Exec(int rank, int* clocks, int size);
void Report_Rec(int rank, int sendrank, char* msg, int* clocks, int size);
void Report_Send(int rank, int receiverank, char* msg, int* clocks, int size);
void Manager();
void Sim_Process(int rank, int size);

/* Main Method
 * 
 * boots up the program, calls 
 * methods based on process rank
 * for further execution, and 
 * cleans up with MPI_Finalize()
 */
int main(int argc, char* argv[]){
  /*Local Variables */
  int rank;
  int size;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  if(rank == 0) { //Manager Process
    Manager();
  } else {//Simulation Processes
    Sim_Process(rank, size);
  }
  
  MPI_Finalize();
}

/* 
 * Print_Clocks
 *
 * helper function that prints
 * all the variables in the clock
 * array (brackets not included
 */
void Print_Clocks(int size, int* clocks){
  for(int i = 1; i < size; i++){
   if(i != (size-1))
      fprintf(stdout, "%d,", clocks[i]);
   else
     fprintf(stdout, "%d", clocks[i]);
  }
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
 * Digits
 * 
 * @returns number of digits needed to
 * represent the integer i.
 */
int Digits(int i) {
  char temp[10];
  sprintf(temp, "%d", i);
  return strlen(temp);
}

/* Create_Clocks
 * Allocates storage and initializes a pointer to
 * an array of integers
 */
int* Create_Clocks(int size){
  int* clocks = malloc(size * sizeof(int));
  for(int i=0; i < size; i++){
    clocks[i] = 0;
  }
  return clocks;
}

/* Update_Clocks
 * Updates the clocks values with any values in 
 * tmpclocks that are larger
 */
int* Update_Clocks(int* tmpclocks, int* clocks, int size){
  for(int i=0; i < size; i++){
    clocks[i] = Max(tmpclocks[i], clocks[i]);
  }
  return clocks;
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
 * Deserialize_Event
 * 
 * parses a serialized string for information
 * @returns an Event struct 
 */
struct Event Deserialize_Event(char* serial) {
  struct Event e;
  e.type = atoi(serial);
  int index = Digits(e.type) + 1;
  if(e.type == 0) {                    //EXEC "type|sender"
    e.sender = atoi(serial+index);
    e.receiver = -1;
  }
  else if(e.type == 1) {              //SEND "type|sender|receiver|message"
    e.sender = atoi(serial+index);
    index += Digits(e.sender) + 1;
    e.receiver = atoi(serial+index);
    index += Digits(e.receiver) + 1;
    sprintf(e.msg, "%s\0", serial+index);
  }
  return e;
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
void Report_End(int rank, int* clocks, int size) {
  fprintf(stdout, "\t[%d]: Logical Clock = [", rank);
  Print_Clocks(size, clocks);
  fprintf(stdout, "]\n");
}

/*
 * Report_Exec
 * A standard print of logical clock status upon
 * simulating an instruction execution.
 */
void Report_Exec(int rank, int* clocks, int size) {
  fprintf(stdout, "\t[%d]: Execution Event: Logical Clock = [", rank);
  Print_Clocks(size, clocks);
  fprintf(stdout,"]\n");
}

/*
 * Report_Rec
 * A standard print of logical clock status upon
 * simulating a message receive.
 */
void Report_Rec(int rank, int sendrank, char* msg, int* clocks, int size) {
  fprintf(stdout, "\t[%d]: Message Received from %d: Message >%s<: Logical Clock = [", rank, sendrank, msg);
  Print_Clocks(size, clocks);
  fprintf(stdout,"]\n");
}

/*
 * Report_Send
 * A standard print of logical clock status upon
 * simulating a message send.
 */
void Report_Send(int rank, int receiverank, char* msg, int* clocks, int size) {
  fprintf(stdout, "\t[%d]: Message Send to %d: Message >%s<: Logical Clock = [", rank, receiverank, msg);
  Print_Clocks(size, clocks);
  fprintf(stdout,"]\n");
}


/*
 * Manager
 * 
 * execution instruction for the
 * manager of the process simulation
 */
void Manager() {
  int sim_size;
  scanf("%d", &sim_size);
  fprintf(stdout, "[0]: There are %d processes in the system\n", sim_size);

  //Reads in events and sends events to appropriate processes
  struct Event e = Read_Event();

  while(e.type == 0 || e.type == 1) {
    char serial[300];
    int slen;
    Serialize_Event(e, serial, &slen);

    if(e.type == 1) {
      //if send event, send a warning to receiver to prepare
      MPI_Send(&serial, slen, MPI_CHAR, e.receiver, 55, MPI_COMM_WORLD);  //send warning
    }
    MPI_Send(&serial, slen, MPI_CHAR, e.sender, 55, MPI_COMM_WORLD);      //send event
    //read next instruction in the file
    e = Read_Event();

  }
  //SIMULATION ENDING
  sleep(1);
  //send out the 'quit' messages
  for(int p = 1; p < sim_size+1; p++) {
    char end_serial[4] = "end\0";
    MPI_Send(&end_serial, 4, MPI_CHAR, p, 55, MPI_COMM_WORLD);
  }
  //wait for processes to wrap up execution
  MPI_Barrier(MPI_COMM_WORLD);
  //print the ending vector clocks times
  fprintf(stdout, "[0]: Simulation ending\n");
  for(int p = 1; p < sim_size+1; p++) {
    int* lclocks = Create_Clocks(sim_size+1);
    MPI_Status status;
    MPI_Recv(lclocks, sim_size+1, MPI_INT, p, 70, MPI_COMM_WORLD, &status);
    Report_End(p, lclocks, sim_size+1);
  }
}

/*
 * Sim_Process
 * Carries out the tasks of a simulation process.
 */
void Sim_Process(int rank, int size) {
  MPI_Status status;
  int* clocks = Create_Clocks(size);
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
      clocks[rank]++;
      Report_Exec(rank, clocks, size);
    }
    else if(e.type == 1) {//SEND
      if(rank == e.sender) {
        //if current process is the sender (received message from manager)
        clocks[rank]++;
        Report_Send(rank, e.receiver, e.msg, clocks, size);

        char serial[300];
        int slen;
        Serialize_Event(e, serial, &slen);
        //pass the clock array
        MPI_Send(clocks, size, MPI_INT, e.receiver, 70, MPI_COMM_WORLD);
        //pass message:
        MPI_Send(&serial, slen, MPI_CHAR, e.receiver, 60, MPI_COMM_WORLD);
      } 
      else {
        //if current process is the receiver (received message from sender)
        int* tmpclocks = Create_Clocks(size);
        //Wait for message from actual sender:
        MPI_Recv(tmpclocks, size, MPI_INT, e.sender, 70, MPI_COMM_WORLD, &status);
        clocks = Update_Clocks(tmpclocks, clocks, size);
        free(tmpclocks);
	
        MPI_Recv(&input, 300, MPI_CHAR, e.sender, 60, MPI_COMM_WORLD, &status);
        e = Deserialize_Event(input);

        clocks[rank]++;
        Report_Rec(rank, e.sender, e.msg, clocks, size);
      }
    }
    else if(e.type == 2) {//END
      fprintf(stderr, "END message found in wrong location.\n");
      break;
    }
  }
  //Send the Finish Clock Time to Manager
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Send(clocks, size, MPI_INT, 0, 70, MPI_COMM_WORLD);
  free(clocks);
}
