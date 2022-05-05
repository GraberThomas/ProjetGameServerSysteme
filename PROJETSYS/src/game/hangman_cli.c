#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../libs/libmessage/message.h"
#include "../libs/libprojectUtil/projectUtil.h"

#define SERV_IN_FILENO 4
#define SERV_OUT_FILENO 3
#define MSG_ERROR_COMM "Error in communication with the server."

int main(int argc, char **argv){
    fprintf(stderr, "\nLancement de hangman_cli\n");

    // server indicates if arguments are valid or not
    int valid_argv = recv_int(SERV_IN_FILENO);
    if (valid_argv == -1){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(1);
    }
    if (!valid_argv) {
        fprintf(stderr,"Les arguments ne sont pas valides.\n");
        char *error_msg = recv_string(SERV_IN_FILENO);
        if(error_msg == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(1); 
        }
        fprintf(stderr, "%s\n", error_msg);
        exit(2);
    }
    fprintf(stderr, "Je finis normalement\n");
    return 0;
}