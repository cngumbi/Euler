#editor:	editor.o
#	cc -o editor editor.o

#editor.o: editor.c
#	cc -c editor.c

#.PHONY: clean

#clean: 
#	rm editor editor.o


#new makefile for the project
CC=gcc
CFLAGS=-std=c99
DEPS=lib.h struct.h filetype.h reopt.h fileio.h io.h
objects = editor.o


%.o:%.c $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)


editor: $(objects)
	$(CC) -g -o $@ $^ $(CFLAGS)


.PHONY: clean


clean:
	rm -f editor $(objects)