PATH_OUT=./out
PATH_LIBS=libs
PATH_LIB_OUT=$(PWD)/${PATH_OUT}/${PATH_LIBS}
PATH_SRC=./src
CC=gcc
CFLAGS=-Wall -std=c99 -g
LDFLAGS=-g



all : create_folders server client

create_folders : 
	mkdir -p ${PATH_LIB_OUT}
	mkdir -p ${PATH_LIB_OUT}/libmessage
	mkdir -p ${PATH_LIB_OUT}/libprojectUtil
	mkdir -p ${PATH_OUT}/game

# Server dependencies
server : $(PATH_OUT)/server.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $^ -lmessage -lprojectUtil

# Client dependencies
client : $(PATH_OUT)/client.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $^ -lmessage -lprojectUtil




#build the shared libraries
$(PATH_LIB_OUT)/libmessage.so : $(PATH_LIB_OUT)/libmessage/libmessage.o
	$(CC) $(LDFLAGS) -shared -o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libmessage/libmessage.o


$(PATH_LIB_OUT)/libprojectUtil.so : $(PATH_LIB_OUT)/libprojectUtil/projectUtil.o
	$(CC) $(LDFLAGS) -shared -o $(PATH_LIB_OUT)/libprojectUtil.so $(PATH_LIB_OUT)/libprojectUtil/projectUtil.o




#build the .o files
$(PATH_OUT)/server.o : $(PATH_SRC)/server.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(PATH_OUT)/client.o : $(PATH_SRC)/client.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(PATH_LIB_OUT)/libmessage/libmessage.o : $(PATH_SRC)/$(PATH_LIBS)/libmessage/message.c $(PATH_SRC)/$(PATH_LIBS)/libmessage/message.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<


$(PATH_LIB_OUT)/libprojectUtil/projectUtil.o : $(PATH_SRC)/$(PATH_LIBS)/libprojectUtil/projectUtil.c $(PATH_SRC)/$(PATH_LIBS)/libprojectUtil/projectUtil.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<


clean : 
	rm -rf $(PATH_OUT)

mrproper :
	rm -f client
	rm -f server

