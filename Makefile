CFLAGS = -g -Wall -pedantic
CFLAGS_T=-g -Wall -pedantic -pthread

.PHONY: all
all: threaded-webserver poll-webserver post-webserver

threaded-webserver: threaded-webserver.c
	gcc $(CFLAGS_T) -o $@ $^

poll-webserver: poll-webserver.c
	gcc $(CFLAGS) -o $@ $^

post-webserver: post-webserver.c myrequest
	gcc $(CFLAGS) -o $@ $^

myrequest: myrequest.o
	gcc $(CFLAGS) -c -o myrequest.o ./request/myrequest.c

.PHONY: clean
	rm -f threaded-webserver post-webserver poll-webserver
