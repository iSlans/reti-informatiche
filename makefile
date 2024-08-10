
CC := gcc
CFLAGS := -I./lib -Wall -std=c89

all: test
# all: server client other 

test: test.o
	$(CC) $(CFLAGS) -o test test.c lib/*
	
server: server.o
	$(CC) $(CFLAGS) -o server server.c lib/*

client: client.o
	$(CC) $(CFLAGS) -o client client.c

other: other.o
	$(CC) $(CFLAGS) -o other other.c

.PHONY: clean
clean:
	rm *o client server other

