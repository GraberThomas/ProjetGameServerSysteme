#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../libs/libmessage/message.h"
#include "../libs/libprojectUtil/projectUtil.h"

#define SERV_IN_FILENO 4
#define SERV_OUT_FILENO 3
#define MSG_ERROR_COMM "Error in communication with the server."
#define ERROR_CODE_COMM  63

void emptyBuffer(){
    int c = 0;
    while (c != '\n' && c != EOF){
        c = getchar();
    }
}

int verifyInput(char input){
    return ((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')) && input != '\n';
}


int main(int argc, char **argv){
    fprintf(stderr, "\nLancement de hangman_cli\n");

    // server indicates if arguments are valid or not
    int valid_argv = recv_int(SERV_IN_FILENO);
    if (valid_argv == -1){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    if (!valid_argv) {
        fprintf(stderr,"Les arguments ne sont pas valides.\n");
        char *error_msg = recv_string(SERV_IN_FILENO);
        if(error_msg == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM); 
        }
        fprintf(stderr, "%s\n", error_msg);
        exit(2);
    }
    //receive welcome message
    char *string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    free(string);

    // Receive number of tries message
    string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    free(string);

    //receive the message of begin of the game
    string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    free(string);
    char char_input;
    int input_ok;
    int bool_again = 1;
    int nb_choice = 1;
    while(bool_again){
        //receive the current display of the word
        string = recv_string(SERV_IN_FILENO);
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
        input_ok = 0;
        while(input_ok == 0){
            fprintf(stdout, "Choice %d, enter a letter :", nb_choice);
            char_input = getchar();
            if(char_input == '\n'){
                fprintf(stdout, "Invalid letter.\n");
                continue;
            }
            emptyBuffer();
            input_ok = verifyInput(char_input);
            if(!input_ok){
                fprintf(stdout, "Invalid letter.\n");
            }
        }
        //send the letter to the server
        if(send_char(SERV_OUT_FILENO, char_input) != 0){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        input_ok = 0;
        nb_choice++;

        //receive the answer of the server
        string = recv_string(SERV_IN_FILENO);
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
        
        bool_again = recv_int(SERV_IN_FILENO);
        if(bool_again == -1){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
    }
    free(string);

    //receive the message of end of the game
    string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    return 0;
}