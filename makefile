CC = gcc
ARGS = -Wall

all: client

client: PingClient.c
	$(CC) $(ARGS) -o client PingClient.c

clean:
	rm -f *.o client *~
