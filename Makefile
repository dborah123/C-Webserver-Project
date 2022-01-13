CFLAGS=-g -Wall -pedantic -pthread

.PHONY: all
all: threaded-webserver poll-webserver

threaded-webserver: threaded-webserver.c
	gcc $(CFLAGS) -o $@ $^

poll-webserver: poll-webserver.c
	gcc $(CFLAGS) -o $@ $^

.PHONY: clean
	rm -f threaded-webserver poll-webserver
