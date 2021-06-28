CC=gcc
CFLAGS=-Wall
LDFLAGS=-lmosquitto
EXEC=bus
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)


all: $(EXEC)

bus: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.o core

mrproper:
	rm -f $(EXEC)

install:
	cp $(EXEC) ../../apps/bus/

