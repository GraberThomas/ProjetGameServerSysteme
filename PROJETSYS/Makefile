CC=gcc
CFLAGS=-Wall -std=c99 -g
LDFLAGS=-g

all : libmessage.so

libmessage.so : libmessage.o
	$(CC) $(LDFLAGS) -shared -o libmessage.so libmessage.o 

libmessage.o : message.c message.h
	$(CC) $(CFLAGS) -fPIC -c -o libmessage.o message.c 

clean :
	rm -f libmessage.so libmessage.o
