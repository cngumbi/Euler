CC=gcc
CFLAGS=-std=c11
DEPS=display.h calculate.h
OBJS=main.o calculate.o display.o

%.o:%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

euler:$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJS)
