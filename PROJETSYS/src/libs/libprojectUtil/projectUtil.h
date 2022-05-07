#ifndef MACRO_UTIL_H
#define MACRO_UTIL_H
#define SERVER_PID_FILE "/tmp/game_server.pid"
#define PATH_FIFO "/tmp/game_server/"
#define GAME_FIFO "/tmp/game_server.fifo"
#define PATH_GAMES_OUT "./out/game/"
#endif

//convert an int to a string
char *itoa(int i);

//get the path of the fifo
char *getPathFIFO(int pid, int id);

//verify string is a positive integer
int isInt(char *str);

//count the number of lines in a file
int countLines(char *path);

//Return the word with the numLine in the file
char *getWordByNumLine(char *path, int numLine);

//get a random number between 1 and max
int getRandomNumber(int max);