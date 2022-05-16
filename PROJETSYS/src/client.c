#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "libs/libmessage/message.h"
#include "libs/libprojectUtil/projectUtil.h"

/**
    * @brief Print usage
    * @param char** The file name
    * @return void
*/
void showUsage(char **argv){
    fprintf(stderr, "Usage: %s game_name [arguments]...\n", argv[0]);
}

/**
    * @brief Verify the arguments given to the program, and test if the game exists and is executable.
    * @param int The number of arguments
    * @param char** The arguments
    * @return void
*/
void verifyArgs(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing game name\n");
        showUsage(argv);
        exit(1);
    }
    //Construct the path to the game
    size_t size_game_path = strlen(PATH_GAMES_OUT)+ strlen(argv[1])+strlen("_cli");
    char *game_path = malloc(sizeof(char) * (size_game_path + 1));
    strcpy(game_path, PATH_GAMES_OUT);
    strcat(game_path, argv[1]);
    strcat(game_path, "_cli");
    game_path[size_game_path] = '\0';
    //Test if the game exists and is executable
    if (access(game_path, F_OK) == -1) {
        fprintf(stderr, "Game %s not found\n", argv[1]);
        showUsage(argv);
        exit(2);
    }else if(access(game_path, X_OK) == -1){
        fprintf(stderr, "Game %s is not executable\n", argv[1]);
        showUsage(argv);
        exit(3);
    }
    free(game_path);
}

//handler for SIGUSR1, used for waking up the client, received by the server
void handler_usr1(int sig){
    
}

//handler for SIGUSR2, used when a server error occurs
void handler_usr2(int sig){
    write(1,"An server error occured\n", 25);
    exit(20);
}

int main(int argc, char **argv){
    //Block the sigusr1 signal and save the old handler
    sigset_t set;
    sigset_t oldset;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, &oldset);
    pid_t pid_client = getpid();
    pid_t pid_server;
    //Test the number of arguments and verify if the game exists and is executable
    verifyArgs(argc, argv);
    //Verify if the server is running
    if(access(SERVER_PID_FILE, F_OK) != 0){
        fprintf(stderr, "The server is not running\n");
        exit(4);
    }
    if(access(SERVER_PID_FILE, R_OK) != 0){
        fprintf(stderr, "Access denied to access to the server pid file\n");
        exit(5);
    }
    //Open and read the pid of the server
    int fd_pid = open(SERVER_PID_FILE, O_RDONLY);
    if(fd_pid == -1){
        fprintf(stderr, "Error while opening the pid file %s\n", SERVER_PID_FILE);
        perror("open");
        exit(6);
    }
    if(read(fd_pid, &pid_server, sizeof(int)) == -1){
        fprintf(stderr, "Error while reading the pid of the server\n");
        perror("read");
        exit(7);
    }
    //Set the handler for sigusr1 and sigusr2
    struct sigaction actSigUsr1;
    struct sigaction actSigUsr2;
    sigemptyset(&actSigUsr1.sa_mask);
    sigemptyset(&actSigUsr2.sa_mask);
    actSigUsr1.sa_flags = 0;
    actSigUsr2.sa_flags = 0;
    actSigUsr1.sa_handler = handler_usr1;
    actSigUsr2.sa_handler = handler_usr2;
    if(sigaction(SIGUSR1, &actSigUsr1, NULL) == -1){
        fprintf(stderr, "Error while setting the handler for SIGUSR1\n");
        perror("sigaction");
        exit(8);
    }
    if(sigaction(SIGUSR2, &actSigUsr2, NULL) == -1){
        fprintf(stderr, "Error while setting the handler for SIGUSR2\n");
        perror("sigaction");
        exit(9);
    }
    //Wake up the server
    kill(pid_server, SIGUSR1);
    int fd_fifo = open(GAME_FIFO, O_WRONLY);
    if(fd_fifo == -1){
        fprintf(stderr, "Error while opening the fifo\n");
        perror("open");
        exit(11);
    }
    //Send the pid of the client to the server
    if(write(fd_fifo, &pid_client, sizeof(pid_t)) == -1){
        fprintf(stderr, "Error while writing the pid of the client to the server\n");
        perror("write");
        exit(12);
    }
    //Send the name of the game to the server
    if(send_string(fd_fifo, argv[1]) != 0){
        fprintf(stderr, "Error while sending the game name to the server\n");
        exit(13);
    }
    //Send the arguments to the server
    int nb_args = argc - 1;
    if(write(fd_fifo, &nb_args, sizeof(int)) == -1){
        fprintf(stderr, "Error while writing the number of arguments to the server\n");
        perror("write");
        exit(14);
    }
    if(nb_args != 0){
        if(send_argv(fd_fifo, argv+1) != 0){
            fprintf(stderr, "Error while sending the arguments to the server\n");
            exit(15);
        }
    }
    //Pause the client until the server create the fifos
    sigset_t new = oldset;
    sigdelset(&new, SIGUSR1);
    sigsuspend(&new);
    //Now the fifos are created
    sigprocmask(SIG_SETMASK, &oldset, NULL);
    //Create the path to the fifo0 and open it
    char *string = getPathFIFO(pid_client, 0);
    int fd0 = open(string, O_WRONLY);
    free(string);
    string=NULL;
    if(fd0 == -1){
        fprintf(stderr, "Error while opening the fifo\n");
        perror("open");
        exit(16);
    }
    //Create the path to the fifo1 and open it
    string = getPathFIFO(pid_client, 1);
    int fd1 = open(string, O_RDONLY);
    free(string);
    string=NULL;
    if(fd1 == -1){
        fprintf(stderr, "Error while opening the fifo\n");
        perror("open");
        exit(17);
    }
    //Duplicate the files descriptors
    if(dup2(fd0, 3) == -1){
        fprintf(stderr, "Error while duplicating the file descriptor\n");
        perror("dup2");
        exit(18);
    }
    close(fd0);
    if(dup2(fd1, 4) == -1){
        fprintf(stderr, "Error while duplicating the file descriptor\n");
        perror("dup2");
        exit(19);
    }
    close(fd1);
    //Make the path of the game
    char *path_game = calloc(sizeof(int), strlen(PATH_GAMES_OUT)+strlen(argv[1])+strlen("_cli")+1);
    sprintf(path_game, "%s%s_cli", PATH_GAMES_OUT, argv[1]);
    //Execute the game
    execvp(path_game, argv+1);
    //execlp("valgrind", "valgrind", "-s","--leak-check=full", "--show-leak-kinds=all",path_game,"-n","3",NULL);
    fprintf(stdout, "Error while executing the game\n");
    free(path_game);
}