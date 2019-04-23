#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/** Global Variables **/


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
