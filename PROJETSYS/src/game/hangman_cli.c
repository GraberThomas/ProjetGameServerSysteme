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
#define PSEUDO_MAX_SIZE 10

void emptyBuffer(){
    int c = 0;
    while (c != '\n' && c != EOF){
        c = getchar();
    }
}

int verifySizePseudo(char *pseudo){
    int i = 0;
    for( i = 0 ; i <= PSEUDO_MAX_SIZE ; i++){
        if(pseudo[i] == '\n'){
            pseudo[i] = '\0';
            return 1;
        }
    }
    return 0;
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
            input_ok = _isAlpha(char_input);
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
    //receive the information of the result
    int bool_result = recv_int(SERV_IN_FILENO);
    if(bool_result == -1){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    //receive the message of end of the game
    string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    free(string);
    if(bool_result){
        string = recv_string(SERV_IN_FILENO);
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
        free(string);
        char_input = getchar();
        emptyBuffer();
        while(char_input != 'y' && char_input != 'n' && char_input != 'Y' && char_input != 'N'){
            fprintf(stdout, "Error in the input. Must be (y/Y or n/N)\n");
            char_input = getchar();
            emptyBuffer();
        }
        if(send_char(SERV_OUT_FILENO, char_input) != 0){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        if(char_input == 'n' || char_input == 'N'){
            return 0;
        }
        string = recv_string(SERV_IN_FILENO);
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
        free(string);
        bool_again = 1;
        while(bool_again){
            string = recv_string(SERV_IN_FILENO);
            if(string == NULL){
                fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            fprintf(stdout, "%s", string);
            free(string);
            string = malloc(sizeof(char)*PSEUDO_MAX_SIZE+1);
            if(fgets(string, PSEUDO_MAX_SIZE, stdin) == NULL){
                free(string);
                fprintf(stderr, "Error with the input.\n");
                perror("fgets");
                exit(15);
            }
            while(verifySizePseudo(string) == 0){
                emptyBuffer();
                fprintf(stdout, "Error in the input. Must be between 4 and %d characters.\n", PSEUDO_MAX_SIZE);
                fprintf(stdout, "Please retry : ");
                if(fgets(string, PSEUDO_MAX_SIZE, stdin) == NULL){
                    free(string);
                    fprintf(stderr, "Error with the input.\n");
                    perror("fgets");
                    exit(15);
                }
            }
            if(send_string(SERV_OUT_FILENO, string) != 0){
                free(string);
                fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            bool_again = recv_int(SERV_IN_FILENO);
            if(bool_again == -1){
                fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            printf("je passe\n");
            free(string);
            if(bool_again){
                string = recv_string(SERV_IN_FILENO);
                printf("je passe2\n");
                if(string == NULL){
                    fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                    exit(ERROR_CODE_COMM);
                }
                fprintf(stdout, "%s\n", string);
                free(string);
            }

        }
        string = recv_string(SERV_IN_FILENO);
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
    }
    return 0;
}   