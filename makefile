
CC := gcc
CFLAGS := -Wall -std=c99

all: test
# all: server client other 

test: 
	$(CC) $(CFLAGS) -o test Server/test.c Server/*/*.c
	
server: 
	$(CC) $(CFLAGS) -o server Server/main.c Server/*/*.c

client: 
	$(CC) $(CFLAGS) -o client Client/main.c Client/*/*.c

other: 
	$(CC) $(CFLAGS) -o other other.c

.PHONY: clean # makes <make clean> work if exists a conflicting file named "clean"  
clean:
	rm *o test client server other
# rm *o client server other

