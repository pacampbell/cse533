CC = gcc
CFLAGS = -Wall -Werror -g
BINS = client echocli timecli server
# Default to nothing if not provided
DEBUG?=

all: $(BINS)

client: client.o client.h
	$(CC) $(CFLAGS) -o client client.o

client.o: client.c client.h
	$(CC) $(CFLAGS) $(DEBUG) -c client.c

# -mt
server: server.o server.h
	$(CC) $(CFLAGS) -o server server.o -lpthread

server.o: server.c server.h
	$(CC) $(CFLAGS) $(DEBUG) -c server.c

echocli: echocli.o echocli.h
	$(CC) $(CFLAGS) -o echocli echocli.o

echocli.o: echocli.c echocli.h
	$(CC) $(CFLAGS) $(DEBUG) -c echocli.c

timecli: timecli.o timecli.h
	$(CC) $(CFLAGS) -o timecli timecli.o

timecli.o: timecli.c timecli.h
	$(CC) $(CFLAGS) $(DEBUG) -c timecli.c

clean:
	rm -f *.o $(BINS)
