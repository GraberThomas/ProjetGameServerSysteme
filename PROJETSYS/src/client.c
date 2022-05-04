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
    size_t size_game_path = strlen("./out/game/")+ strlen(argv[1])+strlen("_cli");
    char *game_path = malloc(sizeof(char) * (size_game_path + 1));
    strcpy(game_path, "./out/game/");
    strcat(game_path, argv[1]);
    strcat(game_path, "_cli");
    game_path[size_game_path] = '\0';
    printf("%s\n", game_path);
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
    pid_t pid_client = getpid();
    pid_t pid_server;
    verifyArgs(argc, argv);
    if(access(SERVER_PID_FILE, F_OK) != 0){
        fprintf(stderr, "The server is not running\n");
        exit(4);
    }
    if(access(SERVER_PID_FILE, R_OK) != 0){
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
    actSigUsr1.sa_handler = handler_usr1;
    actSigUsr2.sa_handler = handler_usr2;
    sigemptyset(&actSigUsr1.sa_mask);
    sigemptyset(&actSigUsr2.sa_mask);
    actSigUsr1.sa_flags = 0;
    actSigUsr2.sa_flags = 0;
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
    // TODO: sigprocmask to block SIGUSR1 and SIGUSR2
    //block sigUSR1
    // sigset_t set;
    // sigset_t oldset;
    // sigemptyset(&set);
    // sigaddset(&set, SIGUSR1);
    // if(sigprocmask(SIG_BLOCK, &set, &oldset) == -1){
    //     fprintf(stderr, "Error while blocking SIGUSR1\n");
    //     perror("sigprocmask");
    //     exit(10);
    // }
    kill(pid_server, SIGUSR1);
    int fd_fifo = open(GAME_FIFO, O_WRONLY);
    if(fd_fifo == -1){
        fprintf(stderr, "Error while opening the fifo\n");
        perror("open");
        exit(11);
    }
    if(write(fd_fifo, &pid_client, sizeof(pid_t)) == -1){
        fprintf(stderr, "Error while writing the pid of the client to the server\n");
        perror("write");
        exit(12);
    }
    if(send_string(fd_fifo, argv[1]) != 0){
        fprintf(stderr, "Error while sending the game name to the server\n");
        exit(13);
    }
    int nb_args = argc - 2;
    if(write(fd_fifo, &nb_args, sizeof(int)) == -1){
        fprintf(stderr, "Error while writing the number of arguments to the server\n");
        perror("write");
        exit(14);
    }
    if(nb_args != 0){
        if(send_argv(fd_fifo, argv+2) != 0){
            printf("%d\n", send_argv(fd_fifo, argv+2));
            fprintf(stderr, "Error while sending the arguments to the server\n");
            exit(15);
        }
    }
    
    // //wait with sugspend for SIGUSR1
    // sigsuspend(&set);
    // //restore sigUSR1
    // if(sigprocmask(SIG_SETMASK, &oldset, NULL) == -1){
    //     fprintf(stderr, "Error while restoring SIGUSR1\n");
    //     perror("sigprocmask");
    //     exit(11);
    // }
    pause();

    printf("Je continue\n");
    fprintf(stdout,"%s\n%s\n", getPathFIFO(pid_client, 0), getPathFIFO(pid_client, 1));
    int fd0 = open(getPathFIFO(pid_client, 0), O_WRONLY);
    if(fd0 == -1){
        fprintf(stderr, "Error while opening the fifo\n");
        perror("open");
        exit(16);
    }
    int fd1 = open(getPathFIFO(pid_client, 1), O_RDONLY);
    if(fd1 == -1){
        fprintf(stderr, "Error while opening the fifo\n");
        perror("open");
        exit(17);
    }
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
    printf("fd0 : %d , fd1 : %d", fd0, fd1);
    // close(fd_fifo);
    // close(fd_pid);
    fprintf(stdout,"Je recouvre\n");
    execvp("./out/game/test_cli", argv+1);
    fprintf(stdout, "Error while executing the game\n");
}