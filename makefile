euler  : test.O
	cc -o euler test.o

test.o : test.c
	cc -c test.c 
clean : 
	rm euler test.o
