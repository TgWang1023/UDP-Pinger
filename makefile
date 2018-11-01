CC = gcc
ARGS = -Wall

all: PingClient

PingClient: PingClient.c
	$(CC) $(ARGS) -o PingClient PingClient.c

clean:
	rm -f *.o PingClient *~
