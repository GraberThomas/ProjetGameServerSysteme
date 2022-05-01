#ifndef MESSAGE_H
#define MESSAGE_H
/**
 * @brief Send a string via a file descriptor
 * 
 * @param fd The file descriptor to send the string to
 * @param str The string to send
 * @return int The return code
 */
int send_string(int fd, char *str);

/**
 * @brief Receive a string from a file descriptor
 * 
 * @param fd The file descriptor to get the string from
 * @return char * The string received
 */
char *recv_string(int fd);

/**
 * @brief Send an array of strings via a file descriptor
 * 
 * @param fd The file descriptor to get the string from
  * @param *argv[] The array of strings to send
 * @return int The return code
 */
int send_argv(int fd, char *argv[]);

/**
 * @brief Receive an array of strings via a file descriptor
 * 
 * @param fd The file descriptor to get the strings from
 * @return char** The array of strings received
 */
char **recv_argv(int fd);

#endif