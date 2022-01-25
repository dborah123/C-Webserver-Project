CFLAGS = -g -Wall -pedantic
CFLAGS_T=-g -Wall -pedantic -pthread

.PHONY: all
all: threaded-webserver poll-webserver myrequest post-webserver

threaded-webserver: threaded-webserver.c
	gcc $(CFLAGS_T) -o $@ $^

poll-webserver: poll-webserver.c
	gcc $(CFLAGS) -o $@ $^

post-webserver: post-webserver.c myrequest.o
	gcc $(CFLAGS) -o $@ $^

myrequest: ./request/myrequest.c
	gcc $(CFLAGS) -c -o myrequest.o ./request/myrequest.c

.PHONY: clean
	rm -f threaded-webserver post-webserver poll-webserver
