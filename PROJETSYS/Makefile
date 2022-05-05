PATH_OUT=./out
PATH_LIBS=libs
PATH_LIB_OUT=${PATH_OUT}/${PATH_LIBS}
PATH_SRC=./src
PATH_GAME_OUT=${PATH_OUT}/game
PATH_GAME_SRC=${PATH_SRC}/game
CC=gcc
CFLAGS=-Wall -std=c99 -g
LDFLAGS=-g



all : create_folders server client $(PATH_GAME_OUT)/test_serv $(PATH_GAME_OUT)/test_cli $(PATH_GAME_OUT)/hangman_serv $(PATH_GAME_OUT)/hangman_cli

create_folders : 
	@mkdir -p ${PATH_LIB_OUT}
	@mkdir -p ${PATH_LIB_OUT}/libmessage
	@mkdir -p ${PATH_LIB_OUT}/libprojectUtil
	@mkdir -p ${PATH_OUT}/game

# Server dependencies
server : $(PATH_OUT)/server.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage -lprojectUtil

# Client dependencies
client : $(PATH_OUT)/client.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage -lprojectUtil

$(PATH_GAME_OUT)/test_serv : $(PATH_GAME_OUT)/test_serv.o $(PATH_LIB_OUT)/libmessage.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage

$(PATH_GAME_OUT)/test_cli : $(PATH_GAME_OUT)/test_cli.o $(PATH_LIB_OUT)/libmessage.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage

$(PATH_GAME_OUT)/hangman_serv : $(PATH_GAME_OUT)/hangman_serv.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage -lprojectUtil

$(PATH_GAME_OUT)/hangman_cli : $(PATH_GAME_OUT)/hangman_cli.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage -lprojectUtil

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

$(PATH_GAME_OUT)/test_cli.o : $(PATH_GAME_SRC)/test_cli.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_GAME_OUT)/test_serv.o : $(PATH_GAME_SRC)/test_serv.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_GAME_OUT)/hangman_cli.o : $(PATH_GAME_SRC)/hangman_cli.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_GAME_OUT)/hangman_serv.o : $(PATH_GAME_SRC)/hangman_serv.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean : 
	rm -rf $(PATH_OUT)

mrproper :
	rm -f client
	rm -f server

