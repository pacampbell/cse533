CC = gcc
CFLAGS = -Wall -Werror -g
BINS = client echocli timecli server
# Default to nothing if not provided
DEBUG?=
LIBS=-lnsl -lresolv -lsocket

all: $(BINS)

linux: LIBS=-lpthread
linux: $(BINS)

client: client.o client.h
	$(CC) $(CFLAGS) -o client client.o $(LIBS)

client.o: client.c client.h
	$(CC) $(CFLAGS) $(DEBUG) -c client.c

server: server.o server.h
	$(CC) $(CFLAGS) -o server server.o $(LIBS)

server.o: server.c server.h
	$(CC) $(CFLAGS) $(DEBUG) -c server.c

echocli: echocli.o echocli.h
	$(CC) $(CFLAGS) -o echocli echocli.o $(LIBS)

echocli.o: echocli.c echocli.h
	$(CC) $(CFLAGS) $(DEBUG) -c echocli.c

timecli: timecli.o timecli.h
	$(CC) $(CFLAGS) -o timecli timecli.o $(LIBS)

timecli.o: timecli.c timecli.h
	$(CC) $(CFLAGS) $(DEBUG) -c timecli.c

clean:
	rm -f *.o $(BINS)
