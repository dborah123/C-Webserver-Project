CFLAGS=-g -Wall -pedantic -pthread

.PHONY: all
all: threaded-webserver

threaded-webserver: threaded-webserver.c
	gcc $(CFLAGS) -o $@ $^
