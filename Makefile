objects = main.o calculate.o display.o

edit: $(objects)
	cc -o program $(objects)

main.o: main.c calculate.h display.h
	cc -c main.c

display.o: display.c display.h calculate.h
	cc -c display.c

calculate.o: calculate.c calculate.h
	cc -c calculate.c

clean:
	rm edit $(objects)
