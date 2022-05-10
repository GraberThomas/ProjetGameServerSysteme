#ifndef MACRO_UTIL_H
#define MACRO_UTIL_H
#define SERVER_PID_FILE "/tmp/game_server.pid"
#define CLIENT_PID_FILE "/tmp/game_client.pid"
#define PATH_GAME_SERVER "/tmp/game_server/"
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

//Get a random number between 1 and max, because sometimes RAND_MAX is not enough
int getRandomNumber(int max);

// verify if a string contains spaces
int containsSpaces(char *str);

//verify if a letter is between a-z or A-Z
int _isAlpha(char input);
