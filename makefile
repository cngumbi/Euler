CC=gcc
CFLAGS=-std=c11
DEPS=display.h calculate.h
OBJS=test.o calculate.o display.o

%.o:%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test:$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJS)
