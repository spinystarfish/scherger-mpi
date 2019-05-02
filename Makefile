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

all: lamport vector

lamport: lamport.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

vector: vector.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) $(RMFLAGS) *.o *~ $(EXE)

t1:
	$(RUN) $(RUNFLAGS) 4 lamport < test1.txt

t2:
	$(RUN) $(RUNFLAGS) 4 lamport < test2.txt

t3:
	$(RUN) $(RUNFLAGS) 6 lamport < test3.txt

v1:
	$(RUN) $(RUNFLAGS) 4 vector < test1.txt

v2:
	$(RUN) $(RUNFLAGS) 4 vector < test2.txt

v3:
	$(RUN) $(RUNFLAGS) 6 vector < test3.txt
