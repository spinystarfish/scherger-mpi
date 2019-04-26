###########################################################
#
# Simple Makefile for Operating Systems Project 1
# tcush
#
# (C++ Version)
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

