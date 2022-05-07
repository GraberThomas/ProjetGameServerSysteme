#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../libs/libmessage/message.h"
#include "../libs/libprojectUtil/projectUtil.h"

#define MSG_ERROR_ARGUMENTS "Hangman : Error with arguments. Check the output of client. Exit.\n"
#define MSG_ERROR_COMM "Error in communication with the client."
#define MSG_WELCOME "Welcome, you have to guess a word"
#define MSG_BEGIN_GAME "Begin of the game. Good luck !"
#define MSG_WORD_TO_FIND "Word to find : "
#define MSG_UNLIMTED_TRIES "There is no limit number of errors"
#define ERROR_CODE_COMM  63
#define STD_IN 0
#define STD_OUT 1 

struct game{
    int *nb_error_max;
    int nb_current_error;
    char *complete_word;
    char *current_word_display;
};

void initGame(struct game *game, int argc, char *argv[], char *secretWord){
    if(argc == 3) {
        game->nb_error_max = malloc(sizeof(int));
        *game->nb_error_max = atoi(argv[2]);
    }else{
        game->nb_error_max = NULL;
    }
    game->nb_current_error = 0;
    game->complete_word = secretWord;
    game->current_word_display = malloc(sizeof(char) * (strlen(secretWord) + 1));
    fprintf(stderr, "Size of word : %ld\n", strlen(secretWord));
    for(int i = 0 ; i < strlen(secretWord) ; i++) {
        game->current_word_display[i] = '-';
    }
    game->current_word_display[strlen(secretWord)] = '\0';
}

int verifyAnswer(struct game *game, char answer){
    int bool_found = 0;
    for(int i = 0 ; i < strlen(game->complete_word) ; i++) {
        if(game->complete_word[i] == answer) {
            if(game->current_word_display[i] != '-') {
                bool_found = 0;
                break;
            }
            game->current_word_display[i] = answer;
            bool_found = 1;
        }
    }
    return bool_found;
}

int isWin(struct game *game){
    for(int i = 0 ; i < strlen(game->current_word_display) ; i++) {
        if(game->current_word_display[i] == '-') {
            return 0;
        }
    }
    return 1;
}


char *getStringOfCurrentWordToFind(struct game *game){
    char *string = malloc(sizeof(char) * (strlen(MSG_WORD_TO_FIND) + strlen(game->current_word_display) + 3));
    sprintf(string, "%s\"%s\"", MSG_WORD_TO_FIND, game->current_word_display);
    return string;
}

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
    int number_of_the_word = getRandomNumber(nb_words);
    fprintf(stderr, "The number of the word is : %d\n", number_of_the_word);
    // char *secretWord = getWordByNumLine("./out/game/dictionnaire.txt", 135186);
    char *secretWord = getWordByNumLine("./out/game/dictionnaire.txt", number_of_the_word);
    fprintf(stderr, "The secret word is : %s\n", secretWord);
    struct game *game = malloc(sizeof(struct game));
    initGame(game, argc, argv, secretWord);
    fprintf(stderr, "The current word is : %s\n", game->complete_word);
    fprintf(stderr, "The current display is : %s\n", game->current_word_display);
    char *string;
    // Send the welcome message
    if(send_string(STD_OUT, MSG_WELCOME) != 0) {
        fprintf(stderr, MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    //Send the number of tries
    if(game->nb_error_max != NULL) {
        size_t size = strlen("You are allowed ") + strlen(argv[2]) + strlen(" errors\n");
        string = malloc(sizeof(char) * (size + 1));
        strcpy(string, "You are allowed ");
        strcat(string, argv[2]);
        strcat(string, " errors");
        string[size] = '\0';
        if(send_string(STD_OUT, string) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        free(string);
    }
    else {
        if(send_string(STD_OUT, MSG_UNLIMTED_TRIES) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
    }
    //send the message of begining the game
    if(send_string(STD_OUT, MSG_BEGIN_GAME) != 0) {
        fprintf(stderr, MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    char char_input;
    int bool_win = 0;
    while (1) {
        if (send_string(STD_OUT, getStringOfCurrentWordToFind(game)) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        char_input = recv_char(STD_IN);
        if (char_input == -1) {
            fprintf(stderr, MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        if(char_input >= 'A' && char_input <= 'Z') {
            char_input = char_input - 'A' + 'a';
        }
        string = calloc(strlen("Good choice, ") + strlen("you are still entlited to ")+strlen(" errors") + strlen(argv[2]) + 1, sizeof(char));
        if(verifyAnswer(game, char_input) == 1) {
            if(game->nb_error_max == NULL) {
                strcpy(string, "Good choice !");
            }else{
                strcpy(string, "Good choice, you are still entlited to ");
                strcat(string, itoa(*game->nb_error_max - game->nb_current_error));
                strcat(string, " errors");
            }
        }else{
            game->nb_current_error++;
            if(game->nb_error_max == NULL) {
                strcpy(string, "Error !");
            }else{
                strcpy(string, "Error, you are still entlited to ");
                strcat(string, itoa(*game->nb_error_max - game->nb_current_error));
                strcat(string, " errors");
            }
        }
        if(send_string(STD_OUT, string) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            exit(ERROR_CODE_COMM);
        }
        free(string);
        bool_win = isWin(game);

        if(bool_win == 1){
            if(send_int(STD_OUT, 0) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            break;
        }else if(game->nb_current_error >= (game->nb_error_max == NULL ? game->nb_current_error : *game->nb_error_max)){
            if(send_int(STD_OUT, 0) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            break;
        }else{
            if(send_int(STD_OUT, 1) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
        }
    }
    if(bool_win == 1) {
        size_t size = strlen("You won, you found the word \"") + strlen(game->complete_word) +strlen("\", you have ") +strlen(itoa(game->nb_current_error) + strlen(" errors"));
        fprintf(stderr, "The size of the string is : %ld\n", size);
        string = calloc(size +1, sizeof(char));
        strcpy(string, "You won, you found the word \"");
        strcat(string, game->complete_word);
        strcat(string, "\", you have ");
        strcat(string, itoa(game->nb_current_error));
        strcat(string, " errors");
        fprintf(stderr, "Win !");
    }else{
        size_t size = strlen("You loose, the secret word was \"") + strlen(game->complete_word) + strlen("\".");
        string = calloc(size +1, sizeof(char));
        strcpy(string, "You loose, the secret word was \"");
        strcat(string, game->complete_word);
        strcat(string, "\".");
    }
    if (send_string(STD_OUT, string) != 0) {
        fprintf(stderr, MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    return 0;
} 