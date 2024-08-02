
CC := gcc
CFLAGS := -Ilib -Wall -std=c89

all: server client other
	
server: server.o
	$(CC) $(CFLAGS) -o server server.c

client: client.o
	$(CC) $(CFLAGS) -o client client.c

other: other.o
	$(CC) $(CFLAGS) -o other other.c

.PHONY: clean
clean:
	rm *o client server other

