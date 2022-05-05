#ifndef MESSAGE_H
#define MESSAGE_H
/**
 * @brief Send a string via a file descriptor
 * 
 * @param fd The file descriptor to send the string to
 * @param str The string to send
 * @return int The return code : 
 *                  3 for a NULL string
 *                  1 for error on writting size of string
 *                  2 for error on writting the string
 *                  0 if sucess
 */
int send_string(int fd, char *str);

/**
 * @brief Receive a string from a file descriptor
 * 
 * @param fd The file descriptor to get the string from
 * @return char * The string received, NULL if error
 */
char *recv_string(int fd);

/**
 * @brief Send an integer via a file descriptor
 * 
 * @param fd The file descriptor to send the integer to
 * @param num The integer to send
 * @return int The return code : 
 *                  1 if error while sending the integer
 *                  0 if sucess
 */
int send_int(int fd, int num);

/**
 * @brief Receive an integer from a file descriptor
 * 
 * @param fd The file descriptor to get the integer from
 * @return int The integer received, -1 if error
 */
int recv_int(int fd);

/**
 * @brief Send an array of strings via a file descriptor
 * 
 * @param fd The file descriptor to get the string from
  * @param *argv[] The array of strings to send
 * @return int The return code :
 *                  4 if the table is empty
 *                  5 for error on writting size of argv
 *                  3 for a NULL string
 *                  1 for error on writting size of string
 *                  2 for error on writting the string
 *                  0 if sucess
 */
int send_argv(int fd, char *argv[]);

/**
 * @brief Receive an array of strings via a file descriptor
 * 
 * @param fd The file descriptor to get the strings from
 * @return char** The array of strings received, NULL if error
 */
char **recv_argv(int fd);


#endif