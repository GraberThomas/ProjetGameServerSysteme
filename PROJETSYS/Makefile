PATH_LIBS=./libs
CC=gcc
CFLAGS=-Wall -std=c99 -g
LDFLAGS=-g

all : server client

client : client.o $(PATH_LIBS)/libmessage.so $(PATH_LIBS)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L${PWD}/libs/ -o client client.o -lmessage -lprojectUtil

server : server.o $(PATH_LIBS)/libmessage.so $(PATH_LIBS)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L${PWD}/libs/ -o server server.o -lmessage -lprojectUtil

client.o : client.c
	$(CC) $(CFLAGS) -c -o client.o client.c 

server.o : server.c
	$(CC) $(CFLAGS) -c -o server.o server.c

libs/libprojectUtil.so : $(PATH_LIBS)/libprojectUtil/projectUtil.o
	$(CC) $(LDFLAGS) -shared -o $(PATH_LIBS)/libprojectUtil.so $(PATH_LIBS)/libprojectUtil/projectUtil.o

libs/libprojectUtil/projectUtil.o : $(PATH_LIBS)/libprojectUtil/projectUtil.c
	$(CC) $(CFLAGS) -fPIC -c -o $(PATH_LIBS)/libprojectUtil/projectUtil.o $(PATH_LIBS)/libprojectUtil/projectUtil.c

libs/libmessage.so : $(PATH_LIBS)/libmessage/libmessage.o
	$(CC) $(LDFLAGS) -shared -o $(PATH_LIBS)/libmessage.so $(PATH_LIBS)/libmessage/libmessage.o 

libs/libmessage/libmessage.o : $(PATH_LIBS)/libmessage/message.c $(PATH_LIBS)/libmessage/message.h
	$(CC) $(CFLAGS) -fPIC -c -o $(PATH_LIBS)/libmessage/libmessage.o $(PATH_LIBS)/libmessage/message.c 

clean :
	rm -f *.o *.so $(PATH_LIBS)/libmessage/*.o $(PATH_LIBS)/*.so $(PATH_LIBS)/libprojectUtil/*.o 
