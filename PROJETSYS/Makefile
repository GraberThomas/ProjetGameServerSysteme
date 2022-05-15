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

# Create folders for output
create_folders : 
	@mkdir -p ${PATH_LIB_OUT}
	@mkdir -p ${PATH_LIB_OUT}/libmessage
	@mkdir -p ${PATH_LIB_OUT}/libprojectUtil
	@mkdir -p ${PATH_OUT}/game
	@cp $(PATH_GAME_SRC)/dictionnaire.txt ${PATH_GAME_OUT}/dictionnaire.txt

#--------------------------------BUILD THE EXECUTABLE-------------------------------------------
# Server
server : $(PATH_OUT)/server.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage -lprojectUtil

# Client
client : $(PATH_OUT)/client.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage -lprojectUtil

#--------------------------------BUILD THE GAME-------------------------------------------
#test_serv
$(PATH_GAME_OUT)/test_serv : $(PATH_GAME_OUT)/test_serv.o $(PATH_LIB_OUT)/libmessage.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage
#test_cli
$(PATH_GAME_OUT)/test_cli : $(PATH_GAME_OUT)/test_cli.o $(PATH_LIB_OUT)/libmessage.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage
#hangman_serv
$(PATH_GAME_OUT)/hangman_serv : $(PATH_GAME_OUT)/hangman_serv.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage -lprojectUtil
#hangman_cli
$(PATH_GAME_OUT)/hangman_cli : $(PATH_GAME_OUT)/hangman_cli.o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libprojectUtil.so
	$(CC) $(LDFLAGS) -L$(PATH_LIB_OUT) -o $@ $< -lmessage -lprojectUtil

#--------------------------------BUILD THE SHARED LIBRARIES-------------------------------------------
#libmessage
$(PATH_LIB_OUT)/libmessage.so : $(PATH_LIB_OUT)/libmessage/libmessage.o
	$(CC) $(LDFLAGS) -shared -o $(PATH_LIB_OUT)/libmessage.so $(PATH_LIB_OUT)/libmessage/libmessage.o

#libprojectUtil
$(PATH_LIB_OUT)/libprojectUtil.so : $(PATH_LIB_OUT)/libprojectUtil/libprojectUtil.o
	$(CC) $(LDFLAGS) -shared -o $(PATH_LIB_OUT)/libprojectUtil.so $(PATH_LIB_OUT)/libprojectUtil/libprojectUtil.o

# $(PATH_LIB_OUT)/lib%.so : $(PATH_LIB_OUT)/lib%/lib%.o
# 	$(CC) $(LDFLAGS) -shared -o $@ $<

#------------------------------------BUILD THE .o FILES-----------------------------------------------
#server
$(PATH_OUT)/server.o : $(PATH_SRC)/server.c
	$(CC) $(CFLAGS) -c -o $@ $<
#client
$(PATH_OUT)/client.o : $(PATH_SRC)/client.c
	$(CC) $(CFLAGS) -c -o $@ $<

#libmessage
$(PATH_LIB_OUT)/libmessage/libmessage.o : $(PATH_SRC)/$(PATH_LIBS)/libmessage/message.c $(PATH_SRC)/$(PATH_LIBS)/libmessage/message.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

#libprojectUtil
$(PATH_LIB_OUT)/libprojectUtil/libprojectUtil.o : $(PATH_SRC)/$(PATH_LIBS)/libprojectUtil/projectUtil.c $(PATH_SRC)/$(PATH_LIBS)/libprojectUtil/projectUtil.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

#The games
$(PATH_GAME_OUT)/%.o : $(PATH_GAME_SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean : 
	rm -rf $(PATH_OUT)

mrproper :
	rm -f client
	rm -f server

