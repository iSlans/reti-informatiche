
CC := gcc
CFLAGS := -Wall -std=c99

all: server client other 
	
server: 
	$(CC) $(CFLAGS) -o server Server/main.c Server/src/*.c

client: 
	$(CC) $(CFLAGS) -o client Client/main.c Client/src/*.c

other: 
	$(CC) $(CFLAGS) -o other Client/main.c Client/src/*.c

.PHONY: clean # makes <make clean> work if exists a conflicting file named "clean"  
clean:
	rm client server other
# rm *o client server other

