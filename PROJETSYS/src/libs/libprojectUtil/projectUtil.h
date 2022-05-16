#ifndef MACRO_UTIL_H
#define MACRO_UTIL_H
#define SERVER_PID_FILE "/tmp/game_server.pid"
#define PATH_GAME_SERVER "/tmp/game_server/"
#define GAME_FIFO "/tmp/game_server.fifo"
#define PATH_GAMES_OUT "./out/game/"
#endif

/**
 * @brief Convert a integer to a string
 * 
 * @param i The integer to convert
 * @return char*  The string representing the integer
 */
char *itoa(int i);

/**
 * @brief Get the path to the fifo file, when the number id
 * 
 * @param pid The pid of the client, who is in the name of the fifo file
 * @param id 0 or 1, the number of the fifo file
 * @return char* The path to the fifo file
 */
char *getPathFIFO(int pid, int id);

/**
 * @brief verify if the string is a positive integer
 * 
 * @param str  The string to verify
 * @return int 1 if the string is a positive integer, 0 otherwise
 */
int isInt(char *str);

/**
 * @brief Count the number of lines in a file
 * 
 * @param path The path to the file
 * @return int -1 if an error occured, the number of lines otherwise
 */
int countLines(char *path);

/**
 * @brief Return the word with the numLine in the dictionary file
 * 
 * @param path The path to the dictionary file
 * @param numLine The number of the line to return
 * @return char* The corresponding word
 */
char *getWordByNumLine(char *path, int numLine);

/**
 * @brief Get a random int between 1 and max, even though max is greater than RAND_MAX
 * 
 * @param max The maximum value of the random int
 * @return int The random int
 */
int getRandomNumber(int max);

/**
 * @brief Verify if the string contains spaces
 * 
 * @param str The string to verify
 * @return int 1 if the string contains spaces, 0 otherwise
 */
int containsSpaces(char *str);

/**
 * @brief Verify if the char is a letter
 * 
 * @param input The char to verify
 * @return int 1 if the char is a letter, 0 otherwise
 */
int _isAlpha(char input);
