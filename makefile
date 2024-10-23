
CC := gcc
CFLAGS := -Wall #-std=c89

all: test
# all: server client other 

test: 
	$(CC) $(CFLAGS) -o test Server/test.c Server/lib/logging.c
	
server: 
	$(CC) $(CFLAGS) -o server Server/server.c Server/lib/logging.c

client: 
	$(CC) $(CFLAGS) -o client Client/client.c

other: 
	$(CC) $(CFLAGS) -o other other.c

.PHONY: clean # <make clean> will work if exists a "clean" named file  
clean:
	rm *o test client server other
# rm *o client server other

