#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/** Global Variables **/


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
