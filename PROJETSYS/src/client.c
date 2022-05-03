#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "libs/libmessage/message.h"
#include "libs/libprojectUtil/projectUtil.h"

void showUsage(char **argv){
    fprintf(stderr, "Usage: ./%s game_name [arguments]...\n", argv[0]);
}

void verifyArgs(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing game name\n");
        showUsage(argv);
        exit(1);
    }
    size_t size_game_path = strlen("./game/")+ strlen(argv[1]);
    char *game_path = malloc(sizeof(char) * (size_game_path + 1));
    strcpy(game_path, "./game/");
    strcat(game_path, argv[1]);
    game_path[size_game_path] = '\0';
    if (access(game_path, F_OK) == -1) {
        fprintf(stderr, "Game %s not found\n", argv[1]);
        showUsage(argv);
        exit(2);
    }else if(access(game_path, X_OK) == -1){
        fprintf(stderr, "Game %s is not executable\n", argv[1]);
        showUsage(argv);
        exit(3);
    }
}

//handler for SIGUSR1
void handler_usr1(int sig){
    printf("SIGUSR1 received\n");
}

//handler for SIGUSR2
void handler_usr2(int sig){
    printf("SIGUSR2 received\n");
}

int main(int argc, char **argv){
    pid_t pid_server;
    verifyArgs(argc, argv);
    if(!access(SERVER_PID_FILE, F_OK)){
        fprintf(stderr, "The server is not running\n");
        exit(4);
    }
    if(!access(SERVER_PID_FILE, R_OK)){
        fprintf(stderr, "Access denied to access to the server pid file\n");
        exit(5);
    }
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
    struct sigaction actSigUsr1;
    struct sigaction actSigUsr2;
    actSigUsr1.sa_handler = SIG_IGN;

}