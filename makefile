euler  : main.O calculate.o display.o
	cc -o euler main.o calculate.o display.o

main.o : main.c calculate.h display.h
	cc -c main.c 

calculate.o : calculate.c calculate.h
	cc -c calculate.c

display.o : display.c display.h
	cc -c display.c

clean :
	rm euler main.o calculate.o display.o
