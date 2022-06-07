<<<<<<< HEAD
CC=gcc
CFLAGS=-std=c11
DEPS=display.h calculate.h
OBJS=test.o calculate.o display.o

%.o:%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test:$(OBJS)
=======
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
>>>>>>> deb9316d6c4247b9239c1a00f284809bac088ec4
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

<<<<<<< HEAD
clean:
	rm -f test $(OBJS)
=======

clean:
	rm -f $(OBJS)

>>>>>>> deb9316d6c4247b9239c1a00f284809bac088ec4
