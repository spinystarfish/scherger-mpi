###########################################################
#
# Simple Makefile for Operating Systems Project 5
# Logical and Vector Clocks using MPI
#
# (C Version)
#
#
###########################################################
.SUFFIXES: .h .c .cpp .l .o

CC = mpicc
RUN = mpirun
CFLAGS = 
RUNFLAGS = -np
LIBS = 
RM = /bin/rm
RMFLAGS = -f

EXE = lamport
OBJS = lamport.c

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) $(RMFLAGS) *.o *~ $(EXE)

test1:
	$(RUN) $(RUNFLAGS) 4 $(EXE) < test1.txt

test2:
	$(RUN) $(RUNFLAGS) 4 $(EXE) < test2.txt

test3:
	$(RUN) $(RUNFLAGS) 6 $(EXE) < test3.txt

