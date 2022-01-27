CFLAGS = -g -Wall -pedantic
CFLAGS_T=-g -Wall -pedantic -pthread

.PHONY: all
all: threaded-webserver poll-webserver myrequest cJSON1 post-webserver

threaded-webserver: threaded-webserver.c
	gcc $(CFLAGS_T) -o $@ $^

poll-webserver: poll-webserver.c
	gcc $(CFLAGS) -o $@ $^

post-webserver: post-webserver.c myrequest.o cJSON.o
	gcc $(CFLAGS) -o $@ $^

myrequest: ./request/myrequest.c
	gcc $(CFLAGS) -c -o myrequest.o ./request/myrequest.c

cJSON1: ./cJSON/cJSON.c
	gcc $(CFLAGS) -c -o cJSON.o ./cJSON/cJSON.c

.PHONY: clean
clear:
	rm -f *.o threaded-webserver post-webserver poll-webserver
