CFLAGS = -g -Wall -pedantic
CFLAGS_T=-g -Wall -pedantic -pthread

.PHONY: all
all: threaded-webserver poll-webserver post-webserver

threaded-webserver: threaded-webserver.c
	gcc $(CFLAGS_T) -o $@ $^

poll-webserver: poll-webserver.c
	gcc $(CFLAGS) -o $@ $^

post-webserver: post-webserver.c
	gcc $(CFLAGS) -o $@ $^

.PHONY: clean
	rm -f threaded-webserver post-webserver poll-webserver
