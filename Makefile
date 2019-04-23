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
RFLAGS = -np
LIBS = 
RM = /bin/rm
RMFLAGS = -f

EXE = lamport
OBJS = lamport.c

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

tcush.o: tcush.cpp
	 $(CXX) $(CXXFLAGS) -c $<

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c $<

lex.yy.c: scan.l
	$(LEX) $<

clean:
	$(RM) $(RMFLAGS) *.o *~ tcush lex.yy.c

test1:
	$(RUN) $(RUNFLAGS) 3 $(EXE) < test1.txt