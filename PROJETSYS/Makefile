$("export LD_LIBRARY_PATH=$PWD")
CC=gcc
CFLAGS=-Wall -std=c99 -g
LDFLAGS=-g

all : libmessage.so testlibmessage_serv testlibmessage_client

testlibmessage_serv : testlibmessage_serv.c libmessage.so
	gcc -g -Wall -std=c99 -L${PWD} -o testlibmessage_serv testlibmessage_serv.c -lmessage

testlibmessage_client : testlibmessage_client.c libmessage.so
	gcc -g -Wall -std=c99 -L${PWD} -o testlibmessage_client testlibmessage_client.c -lmessage	

libmessage.so : libmessage.o
	gcc  -shared -o libmessage.so libmessage.o 

libmessage.o : message.c message.h
	gcc -fPIC -c -o libmessage.o message.c 