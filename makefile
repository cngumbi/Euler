#the makefile of tic tac toe
# define constants CC CFLAGS
# create macros  DEPS which is the set of .h files which .c files depend on.
# create constant OBJS for object files
# $@ left side of :
# $^ right side of :
CC=gcc
CFLAGS=-std=c11
DEPS=tac.h
OBJS=main.o tac.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tac: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean


clean:
	rm -f $(OBJS)
