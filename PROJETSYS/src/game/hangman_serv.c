#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>

#include "../libs/libmessage/message.h"
#include "../libs/libprojectUtil/projectUtil.h"

#define MSG_ERROR_ARGUMENTS "Hangman : Error with arguments. Check the output of client. Exit.\n"
#define MSG_ERROR_COMM "Error in communication with the client."
#define MSG_WELCOME "Welcome, you have to guess a word"
#define MSG_BEGIN_GAME "Begin of the game. Good luck !"
#define MSG_WORD_TO_FIND "Word to find : "
#define MSG_UNLIMTED_TRIES "There is no limit number of errors"
#define MSG_CHOICE_SAVE "Do you want save your result ? [Y/n]"
#define MSG_SAVE "You have chosen to save your result,\nyou must choose a pseudo with no space and minimum length 4\n\n"
#define MSG_SAVE_SUCCESS "Bye, save the result was successful"
#define MSG_SAVE_FAILED "Bye, save the result failed"
#define MSG_ENTER_PSEUDO "Enter your pseudo : "
#define MSG_ENTER_PSEUDO_ERROR "Error : the pseudo is to short, or it contains space(s)"
#define TIMER 4
#define ERROR_CODE_COMM  63
#define STD_IN 0
#define STD_OUT 1 

char *string;
char *string2;
struct game *game;

struct game{
    int *nb_error_max;
    int nb_current_error;
    int nb_tries;
    char *complete_word;
    char *current_word_display;
};

void all_destroy(){
    if(game->nb_error_max != NULL){
        free(game->nb_error_max);
    }
    if(game->complete_word != NULL){
        free(game->complete_word);
    }
    if(game->current_word_display != NULL){
        free(game->current_word_display);
    }
    if(game != NULL){
        free(game);
    }
    if(string != NULL){
        free(string);
    }
    if(string2 != NULL){
        free(string2);
    }
}

//handler for SIGALRM
void sig_handler(int sig){
    int fd_pid_client = open(CLIENT_PID_FILE, O_RDONLY);
    if(fd_pid_client == -1){
        all_destroy();
        fprintf(stderr, "Error when opening the file %s\n", CLIENT_PID_FILE);
        exit(34);
    }
    int pid_client = recv_int(fd_pid_client);
    if(pid_client == -1){
        all_destroy();
        fprintf(stderr, "Error when reading the file %s\n", CLIENT_PID_FILE);
        exit(35);
    }
    if(kill(pid_client, SIGALRM) == -1){
        all_destroy();
        fprintf(stderr, "Error when killing the client\n");
        exit(36);
    }

    all_destroy();
    exit(0);
}

void initGame(struct game *game, int argc, char *argv[], char *secretWord){
    if(argc == 3) {
        game->nb_error_max = malloc(sizeof(int));
        *game->nb_error_max = atoi(argv[2]);
    }else{
        game->nb_error_max = NULL;
    }
    game->nb_current_error = 0;
    game->nb_tries = 0;
    game->complete_word = secretWord;
    game->current_word_display = malloc(sizeof(char) * (strlen(secretWord) + 1));
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
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        if(send_string(STD_OUT,"The number of arguments is not correct") != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        all_destroy();
        exit(1);
    }
    if(argc == 3){
        if(strcmp(argv[1], "-n") != 0){
            if(send_int(STD_OUT, 0) == 1) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            char *a = malloc(sizeof(char ) * (strlen("Error : the option ") + strlen(argv[1]) + strlen(" is not supported\n")+ 1));
            strcpy(a, "Error : the option ");
            strcat(a, argv[1]);
            strcat(a, " is not supported");
            if(send_string(STD_OUT,a) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            free(a);
            a=NULL;
            all_destroy();
            exit(2);
        }
        if( ! isInt(argv[2]) ){
            if(send_int(STD_OUT, 0) == 1) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
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
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            free(a);
            a=NULL;
            all_destroy();
            exit(3);
        }
    }
    if(send_int(STD_OUT, 1) == 1) {
        fprintf(stderr, MSG_ERROR_COMM);
        
        exit(ERROR_CODE_COMM);
    }
}

int saveGameResult(struct game *game, char *pseudo){
    char *pathResults = malloc(sizeof(char) * (strlen(PATH_GAME_SERVER) + strlen("hangman_results") + 1));
    strcpy(pathResults, PATH_GAME_SERVER);
    strcat(pathResults, "hangman_results");
    FILE *file = fopen(pathResults, "a");
    if(file == NULL) {
        free(pathResults);
        return 1;
    }
    //get current date time
    time_t timestamp = time( NULL );
    struct tm * pTime = localtime( & timestamp );
    char *time = malloc(sizeof(char) * 23);
    strftime(time, 23, "%F at %H:%M:%S", pTime);
    time[22] = '\0';
    fprintf(file, "%d;%d;%s;%s;%s\n", game->nb_tries, game->nb_current_error, game->complete_word, pseudo, time);
    free(time);
    time=NULL;
    free(pathResults);
    pathResults=NULL;
    if (fclose(file) != 0) {
        return 1;
    }
    return 0;
}

int main(int argc, char **argv){
    srand(getpid());
    // Verify args sent by client
    verifyArgs(argc, argv);
    int nb_words = countLines("./out/game/dictionnaire.txt");
    if(nb_words == 0) {
        fprintf(stderr, "Error : no words in the dictionary\n");
        all_destroy();
        exit(10);
    }else if(nb_words == -1) {
        fprintf(stderr, "Error : error with the dictionary\n");
        all_destroy();
        exit(11);
    }
    int number_of_the_word = getRandomNumber(nb_words);
    // char *secretWord = getWordByNumLine("./out/game/dictionnaire.txt", 135186);
    char *secretWord = getWordByNumLine("./out/game/dictionnaire.txt", number_of_the_word);
    fprintf(stderr, "Secret word : %s\n", secretWord);
    game = malloc(sizeof(struct game));
    initGame(game, argc, argv, secretWord);
    // Send the welcome message
    if(send_string(STD_OUT, MSG_WELCOME) != 0) {
        fprintf(stderr, MSG_ERROR_COMM);
        all_destroy();
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
            free(string);
            string=NULL;
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        free(string);
        string=NULL;
    }else {
        if(send_string(STD_OUT, MSG_UNLIMTED_TRIES) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
    }
    string2 = itoa(TIMER);
    size_t size = strlen("With each request, you have ") + strlen(string2) + strlen(" seconds to answer\n");
    string = malloc(sizeof(char) * (size + 1));
    strcpy(string, "With each request, you have ");
    strcat(string, string2);
    strcat(string, " seconds to answer");
    string[size] = '\0';
    if(send_string(STD_OUT, string) != 0) {
        fprintf(stderr, MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    free(string);
    string=NULL;
    free(string2);
    string2=NULL;
    //send the message of begining the game
    if(send_string(STD_OUT, MSG_BEGIN_GAME) != 0) {
        fprintf(stderr, MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    char char_input;
    int bool_win = 0;
    struct sigaction sa;
    sa.sa_handler = sig_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    while (1) {
        string = getStringOfCurrentWordToFind(game);
        if (send_string(STD_OUT, string) != 0) {
            free(string);
            string=NULL;
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        free(string);
        string=NULL;
        do {
            alarm(TIMER);
            char_input = recv_char(STD_IN);
            alarm(0);
            if (char_input == -1) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            if(char_input >= 'A' && char_input <= 'Z') {
                char_input = char_input - 'A' + 'a';
            }
        } while (!_isAlpha(char_input));
        game->nb_tries++;
        string = calloc(strlen("Good choice, ") + strlen("you are still entlited to ")+strlen(" errors") + strlen(argv[2]) + 1, sizeof(char));
        
        if(verifyAnswer(game, char_input) == 1) {
            if(game->nb_error_max == NULL) {
                strcpy(string, "Good choice !");
            }else{
                string2 = itoa(*game->nb_error_max - game->nb_current_error);
                strcpy(string, "Good choice, you are still entlited to ");
                strcat(string, string2);
                strcat(string, " errors");
                free(string2);
                string2=NULL;
            }
        }else{
            game->nb_current_error++;
            if(game->nb_error_max == NULL) {
                strcpy(string, "Error !");
            }else {
                string2 = itoa(*game->nb_error_max - game->nb_current_error);
                strcpy(string, "Error, you are still entlited to ");
                strcat(string, string2);
                strcat(string, " errors");
                free(string2);
                string2=NULL;
            }
        }
        if(send_string(STD_OUT, string) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        free(string);
        string=NULL;
        bool_win = isWin(game);

        if(bool_win == 1){
            if(send_int(STD_OUT, 0) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            break;
        }else if(game->nb_error_max != NULL && game->nb_current_error == *game->nb_error_max) {
            if(send_int(STD_OUT, 0) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            break;
        }else{
            if(send_int(STD_OUT, 1) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
        }
    }
    if (send_int(STD_OUT, bool_win) != 0) {
        fprintf(stderr, MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    if(bool_win == 1) {
        string2 = itoa(game->nb_current_error);
        size_t size = strlen("You won, you found the word \"") + strlen(game->complete_word) +strlen("\", you have ") +strlen(string2) + strlen(" errors");
        string = calloc(size +1, sizeof(char));
        strcpy(string, "You won, you found the word \"");
        strcat(string, game->complete_word);
        strcat(string, "\", you have ");
        strcat(string, string2);
        strcat(string, " errors");
        fprintf(stderr, "Win !");
        if(send_string(STD_OUT, string) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        free(string);
        free(string2);
        string=NULL;
        string2=NULL;
        if(send_string(STD_OUT, MSG_CHOICE_SAVE) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        char choice = recv_char(STD_IN);
        if (choice == -1) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        if(choice == 'n' || choice == 'N') {
            all_destroy();
            exit(0);
        }

        // Save the game
        if(send_string(STD_OUT, MSG_SAVE) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }

        char *pseudo;
        while (1) {
            if(send_string(STD_OUT, MSG_ENTER_PSEUDO) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            pseudo = recv_string(STD_IN);
            if (pseudo == NULL) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            if (strlen(pseudo) >= 4 && !containsSpaces(pseudo)) {
                if (send_int(STD_OUT, 0) != 0) {
                    fprintf(stderr, MSG_ERROR_COMM);
                    all_destroy();
                    exit(ERROR_CODE_COMM);
                }
                break;
            }
            free(pseudo);
            pseudo=NULL;
            if (send_int(STD_OUT, 1) != 0) {
                free(pseudo);
                pseudo=NULL;
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            if(send_string(STD_OUT, MSG_ENTER_PSEUDO_ERROR) != 0) {
                free(pseudo);
                pseudo=NULL;
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
        }
        if(saveGameResult(game, pseudo) == 0){
            if(send_string(STD_OUT, MSG_SAVE_SUCCESS) != 0) {
                free(pseudo);
                pseudo=NULL;
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
        }else{
            if(send_string(STD_OUT, MSG_SAVE_FAILED) != 0) {
                free(pseudo);
                pseudo=NULL;
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
        }
        free(pseudo);
        pseudo=NULL;

    }else{
        size_t size = strlen("You loose, the secret word was \"") + strlen(game->complete_word) + strlen("\".");
        string = calloc(size +1, sizeof(char));
        strcpy(string, "You loose, the secret word was \"");
        strcat(string, game->complete_word);
        strcat(string, "\".");
        if (send_string(STD_OUT, string) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
    }
   
    all_destroy();
    exit(0);
} 