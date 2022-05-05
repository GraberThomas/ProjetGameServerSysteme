#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../libs/libmessage/message.h"
#include "../libs/libprojectUtil/projectUtil.h"

#define MSG_ERROR_ARGUMENTS "Hangman : Error with arguments. Check the output of client. Exit.\n"
#define MSG_ERROR_COMM "Error in communication with the client."
#define ERROR_CODE_COMM  63
#define STD_IN 0
#define STD_OUT 1 

void verifyArgs(int argc, char *argv[]) {
    if(argc != 1 && argc != 3) {
        fprintf(stderr, MSG_ERROR_ARGUMENTS);
        if(send_int(STD_OUT, 0) == 1) {
            fprintf(stderr, MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        if(send_string(STD_OUT,"The number of arguments is not correct") != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        exit(1);
    }
    if(argc == 3){
        if(strcmp(argv[1], "-n") != 0){
            if(send_int(STD_OUT, 0) == 1) {
                fprintf(stderr, MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            char *a = malloc(sizeof(char ) * (strlen("Error : the option ") + strlen(argv[1]) + strlen(" is not supported\n")+ 1));
            strcpy(a, "Error : the option ");
            strcat(a, argv[1]);
            strcat(a, " is not supported");
            if(send_string(STD_OUT,a) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            free(a);
            exit(2);
        }
        if( ! isInt(argv[2]) ){
            if(send_int(STD_OUT, 0) == 1) {
                fprintf(stderr, MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            size_t sizeA = strlen("Error : ") + strlen(argv[2]) + strlen(" is not an integer\n");
            char *a = malloc(sizeof(char ) * (sizeA+ 1));
            strcpy(a, "Error : ");
            strcat(a, argv[2]);
            strcat(a, " is not an integer");
            a[sizeA] = '\0';
            if(send_string(STD_OUT,a) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            free(a);
            exit(3);
        }
    }
    if(send_int(STD_OUT, 1) == 1) {
        fprintf(stderr, MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
}

int main(int argc, char **argv){
    srand(getpid());
    fprintf(stderr, "\nLancement de hangman_serv\n");
    // Verify args sent by client
    verifyArgs(argc, argv);
    int nb_words = countLines("./out/game/dictionnaire.txt");
    fprintf(stderr, "Nombre de mots dans le dictionnaire : %d\n", nb_words);
    if(nb_words == 0) {
        fprintf(stderr, "Error : no words in the dictionary\n");
        exit(10);
    }else if(nb_words == -1) {
        fprintf(stderr, "Error : error with the dictionary\n");
        exit(11);
    }
    int number_of_the_word = rand() % nb_words + 1;
    fprintf(stderr, "The number of the word is : %d\n", number_of_the_word);
    char *secretWord = getWordByNumLine("./out/game/dictionnaire.txt", number_of_the_word);
    fprintf(stderr, "The secret word is : %s\n", secretWord);
    return 0;
} 