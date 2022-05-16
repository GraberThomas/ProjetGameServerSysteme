#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "../libs/libmessage/message.h"
#include "../libs/libprojectUtil/projectUtil.h"

#define SERV_IN_FILENO 4
#define SERV_OUT_FILENO 3
#define MSG_ERROR_COMM "Error in communication with the server."
#define ERROR_CODE_COMM  63
#define PSEUDO_MAX_SIZE 10 //The max size of the pseudo

char *string = NULL; //A string for make and use 
int pid_serv; //The pid of the server

/**
 * @brief A function who free the memory of the string
 * 
 */
void all_destroy(void) {
    if(string != NULL) {
        free(string);
    }
}

/**
 * @brief A function whe empty the buffer
 * 
 */
void emptyBuffer(){
    int c = 0;
    while (c != '\n' && c != EOF){
        c = getchar();
    }
}

/**
 * @brief A function who verify the pseudo and replace the '\n' by '\0'
 * 
 * @param pseudo The pseudo entered by the user
 * @return int 1 if the pseudo is valid, 0 otherwise
 */
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

//handler for SIGALRM
void handler_alrm(int sig){
    fprintf(stdout, "\nYou are out of time\n");
    all_destroy();
    exit(0);
}

//handler for sigUSR2
void handler_usr2(int sig){
    fprintf(stderr, "\nThe server is down\nExit\n");
    all_destroy();
    exit(87);
}

//handler for sigINT
void handler_int(int sig){
    fprintf(stdout, "\nExit\n");
    if(kill(pid_serv, SIGUSR2) == -1){
        perror("kill");
        all_destroy();
        exit(97);
    }
    all_destroy();
    exit(0);
}

int main(int argc, char **argv){
    //Set up the signal handlers for sigINT and sigUSR2
    struct sigaction saUSR2;
    struct sigaction saInt;
    saUSR2.sa_handler = handler_usr2;
    saInt.sa_handler = handler_int;
    sigemptyset(&saInt.sa_mask);
    sigemptyset(&saUSR2.sa_mask);
    saUSR2.sa_flags = 0;
    saInt.sa_flags = 0;
    if(sigaction(SIGUSR2, &saUSR2, NULL) == -1){
        perror("sigaction USR2");
        exit(27);
    }
    if(sigaction(SIGINT, &saInt, NULL) == -1){
        perror("sigaction INT");
        exit(27);
    }
    //Receive the pid of the server
    pid_serv = recv_int(SERV_IN_FILENO);
    if(pid_serv == -1){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        all_destroy();
        exit(61);
    }
    //Receive the validity of the arguments
    int valid_argv = recv_int(SERV_IN_FILENO);
    if (valid_argv == -1){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    if (!valid_argv) {
        fprintf(stderr,"Les arguments ne sont pas valides.\n");
        char *error_msg = recv_string(SERV_IN_FILENO);
        if(error_msg == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM); 
        }
        fprintf(stderr, "%s\n", error_msg);
        all_destroy();
        exit(2);
    }
    //receive welcome message
    string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    free(string);
    string=NULL;
    // Receive number of tries message and display it
    string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    free(string);
    string = NULL;
    //receive the timer msg and display it
    string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    free(string);
    string = NULL;
    //receive the message of begin of the game and display it
    string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    free(string);
    string=NULL;
    char char_input; //The char entered by the user
    int input_ok; //The boolean of validity of the input
    int bool_again = 1; //The boolean of the loop
    int nb_choice = 1; //The number of the choice
    //Set up the handler for SIGALRM, receive when the player is out of time
    struct sigaction act;
    act.sa_handler = handler_alrm;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGALRM, &act, NULL) == -1) {
       perror("sigaction");
        all_destroy();
        exit(1);
    }
    //Begin of the loop of play
    while(bool_again){
        //receive the current display of the word and display it
        string = recv_string(SERV_IN_FILENO);
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
        free(string);
        string=NULL;
        input_ok = 0;
        //Begin of the loop of input, loop until the input is not valid
        while(input_ok == 0){
            fprintf(stdout, "Choice %d, enter a letter :", nb_choice);
            char_input = getchar();
            emptyBuffer();
            if(char_input == '\n'){
                fprintf(stdout, "Invalid letter.\n");
                continue;
            }
            if(send_char(SERV_OUT_FILENO, char_input) == -1){ //Send the input to the server
                fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            input_ok = recv_int(SERV_IN_FILENO); //Receive the validity of the input
            if(input_ok == -1){
                fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }else if(input_ok == 0){
                fprintf(stderr, "Invalid letter. Retry.\n");
            }
        }
        nb_choice++;
        //receive the answer of the server and display it
        string = recv_string(SERV_IN_FILENO);
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
        free(string);
        string=NULL;
        bool_again = recv_int(SERV_IN_FILENO);
        if(bool_again == -1){ //Receive if the loop must continue
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
    }
    //receive if the player won or lost
    int bool_result = recv_int(SERV_IN_FILENO);
    if(bool_result == -1){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    //receive the message of end of the game and display it
    string = recv_string(SERV_IN_FILENO);
    if(string == NULL){
        fprintf(stderr, "%s\n",MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    fprintf(stdout, "%s\n", string);
    free(string);
    string = NULL;
    if(bool_result){ //If the player won
        //receive the invitation to save the score and display it
        string = recv_string(SERV_IN_FILENO);
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
        free(string);
        string = NULL;
        char_input = getchar(); //Wait for the user to indicate if he wants to save the score
        emptyBuffer();
        while(char_input != 'y' && char_input != 'n' && char_input != 'Y' && char_input != 'N'){ //Loop until the user enters a valid input
            fprintf(stdout, "Error in the input. Must be (y/Y or n/N)\n");
            char_input = getchar();
            emptyBuffer();
        }
        //Send the input to the server
        if(send_char(SERV_OUT_FILENO, char_input) != 0){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        if(char_input == 'n' || char_input == 'N'){ //If the player dont want to save the score
            all_destroy();
            return 0;
        }
        //Receive the confirmation of the server and display it
        string = recv_string(SERV_IN_FILENO);
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
        free(string);
        string = NULL;
        bool_again = 1;
        //Begin of the loop of saving the score, until the pseudo is valid
        while(bool_again){
            //receive the invitation to enter the pseudo and display it
            string = recv_string(SERV_IN_FILENO);
            if(string == NULL){
                fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            fprintf(stdout, "%s", string);
            free(string);
            string=NULL;
            string = calloc(sizeof(char), PSEUDO_MAX_SIZE+1);
            if(fgets(string, PSEUDO_MAX_SIZE, stdin) == NULL){ // Get the pseudo from the user
                fprintf(stderr, "Error with the input.\n");
                perror("fgets");
                all_destroy();
                exit(15);
            }
            //Loop until the pseudo is valid
            while(verifySizePseudo(string) == 0){
                emptyBuffer();
                fprintf(stdout, "Error in the input. Must be between 4 and %d characters.\n", PSEUDO_MAX_SIZE);
                fprintf(stdout, "Please retry : ");
                if(fgets(string, PSEUDO_MAX_SIZE, stdin) == NULL){
                    fprintf(stderr, "Error with the input.\n");
                    perror("fgets");
                    all_destroy();
                    exit(15);
                }
            }
            //Send the pseudo to the server
            if(send_string(SERV_OUT_FILENO, string) != 0){
                fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            bool_again = recv_int(SERV_IN_FILENO); //Receive if the loop is finished
            if(bool_again == -1){
                fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            free(string);
            string=NULL;
            if(bool_again){
                string = recv_string(SERV_IN_FILENO); //Receive the error message and display it
                if(string == NULL){
                    fprintf(stderr, "%s\n",MSG_ERROR_COMM);
                    all_destroy();
                    exit(ERROR_CODE_COMM);
                }
                fprintf(stdout, "%s\n", string);
                free(string);
                string=NULL;
            }

        }
        string = recv_string(SERV_IN_FILENO); //Receive the message of end of the saving and display it
        if(string == NULL){
            fprintf(stderr, "%s\n",MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        fprintf(stdout, "%s\n", string);
        free(string);
        string=NULL;
    }
    return 0;
}   