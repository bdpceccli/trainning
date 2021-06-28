CC=gcc
CFLAGS=-Wall
LDFLAGS=
EXEC=saphir
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)


all: $(EXEC)

saphir: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.o core

mrproper:
	rm -f $(EXEC)


