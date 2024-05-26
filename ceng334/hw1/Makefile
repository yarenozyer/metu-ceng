CC=gcc
CFLAGS=-g

all: bgame

bgame: bgame.c logging.c message.c
	$(CC) $(CFLAGS) -o bgame bgame.c logging.c message.c

clean:
	rm -f bgame
