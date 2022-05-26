
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
#define MSG_ENTER_PSEUDO_ERROR "Error : the pseudo is too short, too long, or it contains space(s)"
#define TIMER 10 //The maximum time the player must take to respond
#define ERROR_CODE_COMM  63
#define STD_IN 0
#define STD_OUT 1 

//2 variable who will be used for using string
char *string; 
char *string2;
struct game *game;
pid_t pid_client;

struct game{ //A sruct who represent the game
    int *nb_error_max; //The maximum number of error
    int nb_current_error; //The current number of error
    int nb_tries; //The number of tries
    char *complete_word; //The word to find
    char *current_word_display; //The current display of the word
};

/**
 * @brief Free all variables used in the program
 */
void all_destroy(){
    if(game != NULL){
        if(game->nb_error_max != NULL){
            free(game->nb_error_max);
        }
        if(game->complete_word != NULL){
            free(game->complete_word);
        }
        if(game->current_word_display != NULL){
            free(game->current_word_display);
        }
        free(game);
    }
    if(string != NULL){
        free(string);
    }
    if(string2 != NULL){
        free(string2);
    }
}

//handler for SIGALRM, received when the player is out of time
void sig_handler(int sig){
    //Send sigALRM to the client
    if(kill(pid_client, SIGALRM) == -1){
        all_destroy();
        fprintf(stderr, "Error when killing the client\n");
        exit(36);
    }
    all_destroy();
    exit(0);
}

/**
 * @brief Function who initialize the struct game
 * 
 * @param game The struct game
 * @param argc The number of arguments
 * @param argv The arguments
 * @param secretWord The word to find by player
 */
void initGame(struct game *game, int argc, char *argv[], char *secretWord){
    if(argc == 4) {
        game->nb_error_max = malloc(sizeof(int));
        *game->nb_error_max = atoi(argv[3]);
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

/**
 * @brief The function for verify the string entered by the player
 * 
 * @param game The sruct game
 * @param answer The string entered by the player
 * @return int 1 if the string is correct, 0 if not
 */
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

/**
 * @brief Boolean function who verify is the player win the game
 * 
 * @param game The struct game
 * @return int 1 if the player win, 0 if not
 */
int isWin(struct game *game){
    for(int i = 0 ; i < strlen(game->current_word_display) ; i++) {
        if(game->current_word_display[i] == '-') {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Make the string representing the current display of the word
 * 
 * @param game The struct game
 * @return char* The string representing the current display of the word
 */
char *getStringOfCurrentWordToFind(struct game *game){
    char *string = malloc(sizeof(char) * (strlen(MSG_WORD_TO_FIND) + strlen(game->current_word_display) + 3));
    sprintf(string, "%s\"%s\"", MSG_WORD_TO_FIND, game->current_word_display);
    return string;
}

/**
 * @brief Verify the validity of the arguments entered by the player in client
 * 
 * @param argc The number of arguments
 * @param argv The arguments
 */
void verifyArgs(int argc, char *argv[]) {
    // for(int i = 1 ; i < argc ; i++) {
    //     if(strcmp(argv[i], "-h") == 0) {
    //         printf("%s", MSG_ERROR_ARGUMENTS);
    //         exit(0);
    //     }
    // }
    //Test the number of arguments
    if(argc != 2 && argc != 4) {
        fprintf(stderr, MSG_ERROR_ARGUMENTS);
        if(send_int(STD_OUT, 0) == 1) { //Send to client the arguments is not valid
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
    if(argc == 4){
        //-n is the only option
        if(strcmp(argv[2], "-n") != 0){ 
            if(send_int(STD_OUT, 0) == 1) { //Send to client the arguments is not valid
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            //Make the string of the error and send it to the client
            char *a = malloc(sizeof(char ) * (strlen("Error : the option ") + strlen(argv[2]) + strlen(" is not supported\n")+ 1));
            strcpy(a, "Error : the option ");
            strcat(a, argv[2]);
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
        //The number after -n must be a positive int
        if( ! isInt(argv[3]) ){
            if(send_int(STD_OUT, 0) == 1) {  //Send to client the arguments is not valid
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            //Make the string of the error and send it to the client
            size_t sizeA = strlen("Error : ") + strlen(argv[3]) + strlen(" is not an integer\n");
            char *a = malloc(sizeof(char ) * (sizeA+ 1));
            strcpy(a, "Error : ");
            strcat(a, argv[3]);
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
    if(send_int(STD_OUT, 1) == 1) { //Send to client the arguments is valid
        fprintf(stderr, MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
}

/**
 * @brief This function save the result in the file
 * 
 * @param game The struct game
 * @param pseudo The pseudo choosen by the player
 * @return int 0 if an error occured, 1 if not
 */
int saveGameResult(struct game *game, char *pseudo){
    //make the path of the file to save the result
    char *pathResults = malloc(sizeof(char) * (strlen(PATH_GAME_SERVER) + strlen("hangman_results") + 1));
    strcpy(pathResults, PATH_GAME_SERVER);
    strcat(pathResults, "hangman_results");
    //open the file
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
    //write the result in the file
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
//handler for SIGINT
void handler_sigint(int sig){
    if(kill(pid_client, SIGUSR2) == -1) {
        fprintf(stderr, "%d : Error : kill %d\n", getpid(), pid_client);
        exit(ERROR_CODE_COMM);
    }
    all_destroy();
    exit(89);
}

//handler for SIGUSR2
void handler_sigusr2(int sig){
    fprintf(stderr, "%d : Error : Client disconnected\n", getpid());
    all_destroy();
    exit(90);
}

int main(int argc, char **argv){
    //Set the handler for SIGINT
    struct sigaction saINT;
    saINT.sa_handler = handler_sigint;
    sigemptyset(&saINT.sa_mask);
    saINT.sa_flags = 0;
    if(sigaction(SIGINT, &saINT, NULL) == -1) {
        fprintf(stderr, "%d : Error : sigaction\n", getpid());
        perror("sigaction");
        exit(27);
    }
    if(sigaction(SIGTERM, &saINT, NULL) == -1) {
        fprintf(stderr, "%d : Error : sigaction\n", getpid());
        perror("sigaction");
        exit(27);
    }
    if(sigaction(SIGQUIT, &saINT, NULL) == -1) {
        fprintf(stderr, "%d : Error : sigaction\n", getpid());
        perror("sigaction");
        exit(27);
    }
    if(sigaction(SIGINT, &saINT, NULL) == -1) {
        fprintf(stderr, "%d : Error : sigaction\n", getpid());
        perror("sigaction");
        exit(27);
    }
    //Set the handler for SIGUSR2
    struct sigaction saUSR2;
    saUSR2.sa_handler = handler_sigusr2;
    sigemptyset(&saUSR2.sa_mask);
    saUSR2.sa_flags = 0;
    if(sigaction(SIGUSR2, &saUSR2, NULL) == -1) {
        fprintf(stderr, "%d : Error : sigaction\n", getpid());
        perror("sigaction");
        exit(27);
    }
    pid_client = atoi(argv[1]);
    //Send the pid of this server to the corresponding client
    if(send_int(STD_OUT, getpid()) == 1) {
        fprintf(stderr, MSG_ERROR_COMM);
        exit(ERROR_CODE_COMM);
    }
    //Initialize the random generator
    srand(getpid());
    // Verify args sent by client
    verifyArgs(argc, argv);
    //Get the number of word of the dictionnary
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
    //Get the number of the line of the word to play
    int number_of_the_word = getRandomNumber(nb_words);
    //Get the corresponding word
    char *secretWord = getWordByNumLine("./out/game/dictionnaire.txt", number_of_the_word);
    fprintf(stderr, "Secret word : %s\n", secretWord);
    //Create the struct game and initialize it
    game = malloc(sizeof(struct game));
    initGame(game, argc, argv, secretWord);
    // Send the welcome message to the client
    if(send_string(STD_OUT, MSG_WELCOME) != 0) {
        fprintf(stderr, MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    //Send the message of the number max of error to the client
    if(game->nb_error_max != NULL) { //A number max is choosen by the player
        size_t size = strlen("You are allowed ") + strlen(argv[3]) + strlen(" errors\n");
        string = malloc(sizeof(char) * (size + 1));
        strcpy(string, "You are allowed ");
        strcat(string, argv[3]);
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
    }else { //No number max is unlimited
        if(send_string(STD_OUT, MSG_UNLIMTED_TRIES) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
    }
    //Make and send the message of the time the player has to respond
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
    char char_input; //The char input by the player
    int bool_win = 0; //The boolean that indicates if the player won
    //Set up the handler for sigalrm
    struct sigaction sa;
    sa.sa_handler = sig_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    //Begin of the loop of gameplay
    while (1) {
        string = getStringOfCurrentWordToFind(game); //Get the string of the current word to find and send it
        if (send_string(STD_OUT, string) != 0) {
            free(string);
            string=NULL;
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        free(string);
        string=NULL;
        //begin of the loop of the player's input, loop while char_input is not valid
        do {
            //set up the timer
            alarm(TIMER);
            char_input = recv_char(STD_IN);
            alarm(0);
            if (char_input == -1) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            if(! _isAlpha(char_input)) {
                if(send_int(STD_OUT, 0) != 0) { //Send that the input is not valid to the client
                    fprintf(stderr, MSG_ERROR_COMM);
                    all_destroy();
                    exit(ERROR_CODE_COMM);
                }
                continue;
            }else{
                if(send_int(STD_OUT, 1) != 0) { //Send that the input is valid to the client
                    fprintf(stderr, MSG_ERROR_COMM);
                    all_destroy();
                    exit(ERROR_CODE_COMM);
                }
            }
            //convert the char to lowercase
            if(char_input >= 'A' && char_input <= 'Z') {
                char_input = char_input - 'A' + 'a';
            }
        } while (!_isAlpha(char_input));
        game->nb_tries++;
        //Allocate the memory for the string of the answer
        string = calloc(strlen("Good choice, ") + strlen("you are still entlited to ")+strlen(" errors") + strlen(argv[3]) + 1, sizeof(char));
        //Verify the char and make and send the message of the answer
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
        if(bool_win == 1){ //If the player won
            if(send_int(STD_OUT, 0) != 0) { //Send that the loop is over to the client
                fprintf(stderr, MSG_ERROR_COMM);
                exit(ERROR_CODE_COMM);
            }
            break;
        }else if(game->nb_error_max != NULL && game->nb_current_error == *game->nb_error_max) { //If the player lost
            if(send_int(STD_OUT, 0) != 0) { //Send that the loop is over to the client
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            break;
        }else{
            if(send_int(STD_OUT, 1) != 0) { //Send that the loop is not over to the client
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
        }
    }
    //send to the client if the player won
    if (send_int(STD_OUT, bool_win) != 0) {
        fprintf(stderr, MSG_ERROR_COMM);
        all_destroy();
        exit(ERROR_CODE_COMM);
    }
    if(bool_win == 1) { //If the player won
        //Make the string of the message of the win and send it
        string2 = itoa(game->nb_current_error);
        size_t size = strlen("You won, you found the word \"") + strlen(game->complete_word) +strlen("\", you have ") +strlen(string2) + strlen(" errors");
        string = calloc(size +1, sizeof(char));
        strcpy(string, "You won, you found the word \"");
        strcat(string, game->complete_word);
        strcat(string, "\", you have ");
        strcat(string, string2);
        strcat(string, " errors");
        if(send_string(STD_OUT, string) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        free(string);
        free(string2);
        string=NULL;
        string2=NULL;
        //Ask the player if he wants to save the result
        if(send_string(STD_OUT, MSG_CHOICE_SAVE) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        char choice = recv_char(STD_IN); //receive the choice of the player
        if (choice == -1) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }
        if(choice == 'n' || choice == 'N') { //If the player doesn't want to save the result, exit the program
            all_destroy();
            exit(0);
        }

        //Send the confirmation of the choice to the client
        if(send_string(STD_OUT, MSG_SAVE) != 0) {
            fprintf(stderr, MSG_ERROR_COMM);
            all_destroy();
            exit(ERROR_CODE_COMM);
        }

        //A loop which loop until the player send a valid pseudo
        char *pseudo;
        while (1) {
            //Invite the player to enter a pseudo
            if(send_string(STD_OUT, MSG_ENTER_PSEUDO) != 0) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            //Receive the pseudo
            pseudo = recv_string(STD_IN);
            if (pseudo == NULL) {
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            //Verify if the pseudo is valid
            if (strlen(pseudo) >= 4 && strlen(pseudo) <= 10 && !containsSpaces(pseudo)) {
                if (send_int(STD_OUT, 0) != 0) { //Send to the client that the loop is over
                    fprintf(stderr, MSG_ERROR_COMM);
                    all_destroy();
                    exit(ERROR_CODE_COMM);
                }
                break;
            }
            free(pseudo);
            pseudo=NULL;
            if (send_int(STD_OUT, 1) != 0) { //Send to the client that the loop is not over
                free(pseudo);
                pseudo=NULL;
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
            if(send_string(STD_OUT, MSG_ENTER_PSEUDO_ERROR) != 0) { //Send the error message to the client
                free(pseudo);
                pseudo=NULL;
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
        }
        if(saveGameResult(game, pseudo) == 0){ //Save the result of the game
            if(send_string(STD_OUT, MSG_SAVE_SUCCESS) != 0) { //Send the success message to the client
                free(pseudo);
                pseudo=NULL;
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
        }else{
            if(send_string(STD_OUT, MSG_SAVE_FAILED) != 0) { //Send the failed message to the client
                free(pseudo);
                pseudo=NULL;
                fprintf(stderr, MSG_ERROR_COMM);
                all_destroy();
                exit(ERROR_CODE_COMM);
            }
        }
        free(pseudo);
        pseudo=NULL;

    }else{ //If the player lost
        //Make the string of the message of the lose and send it 
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