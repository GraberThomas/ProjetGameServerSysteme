//CODE ERREUR142
//BUG AVEC LES pid_childs

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>


#include "libs/libmessage/message.h"
#include "libs/libprojectUtil/projectUtil.h"

volatile int usr1_receive = 0;
int pid_client;

/**
    * @brief Check if the given file exists
    * @param filename The file name
*/
int checkIfFileExists(const char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else  
        return 0;
}

/**
    * @brief Stop the server properly
    * @param int error The error code
    * @param char* message The message to display
    * @return void  
*/
int serv_exit(int error, char *msg) {
    if((unlink(SERVER_PID_FILE)) == -1){
        fprintf(stderr, "Error deleting server pid file\n");
        perror("unlink");
    }
    fprintf(stderr, "%s\n", msg);
    exit(error);
}

void handSIGUSR1(int sig) {
    usr1_receive = 1;
}

//handler for sigint
void handSIGINT(int sig){
    //block SIGCHLD
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    int pid = 0;
    int result;
    while(pid != -1){
        pid = wait(&result);
        if(pid == -1){
            break;
        }
        if (WIFEXITED(result)) {
            fprintf(stderr,"%d : exited, status=%d\n",pid, WEXITSTATUS(result));
        } else if (WIFSIGNALED(result)) {
            fprintf(stderr, "%d : killed by signal %d\n",pid, WTERMSIG(result));
        } else if (WIFSTOPPED(result)) {
            fprintf(stderr, "%d : stopped by signal %d\n",pid, WSTOPSIG(result));
        } else if (WIFCONTINUED(result)) {
            fprintf(stderr, "%d : continued\n",pid);
        }
    }
    serv_exit(0, "\nServer interrupted by user\n");
}

//handler for sigchld
void handSIGCHLD(int sig){
    int result;
    int pid = wait(&result);
    if(pid == -1){
        perror("wait");
        serv_exit(1, "Error in wait\n");
    }
    if (WIFEXITED(result)) {
        fprintf(stderr, "%d : exited, status=%d\n",pid, WEXITSTATUS(result));
    } else if (WIFSIGNALED(result)) {
        fprintf(stderr, "%d : killed by signal %d\n",pid, WTERMSIG(result));
    } else if (WIFSTOPPED(result)) {
        fprintf(stderr, "%d : stopped by signal %d\n",pid, WSTOPSIG(result));
    } else if (WIFCONTINUED(result)) {
        fprintf(stderr, "%d : continued\n",pid);
    }
}


int main(int argc, char **argv){
    if(checkIfFileExists(SERVER_PID_FILE)){
        fprintf(stderr, "The server is already running\n");
        exit(1);
    }
    struct sigaction actCHILD;
    actCHILD.sa_handler = handSIGCHLD;
    actCHILD.sa_flags = 0;
    sigemptyset(&actCHILD.sa_mask);
    if(sigaction(SIGCHLD, &actCHILD, NULL) == -1){
        fprintf(stderr, "Error setting SIGCHLD handler\n");
        perror("sigaction");
        exit(45);
    }
    struct sigaction actINT;
    sigemptyset(&actINT.sa_mask);
    actINT.sa_flags = 0;
    actINT.sa_handler = handSIGINT;
    if(sigaction(SIGINT, &actINT, NULL) == -1){
        write(2, "Error setting SIGINT handler\n", 27);
        perror("sigaction");
        serv_exit(1, "Error setting SIGINT handler\n");
    }
    if(sigaction(SIGQUIT, &actINT, NULL) == -1){
        write(2, "Error setting SIGINT handler\n", 27);
        perror("sigaction");
        serv_exit(1, "Error setting SIGINT handler\n");
    }
    if(sigaction(SIGTERM, &actINT, NULL) == -1){
        write(2, "Error setting SIGINT handler\n", 27);
        perror("sigaction");
        serv_exit(1, "Error setting SIGINT handler\n");
    }
    int fd_fifo_game_server_pid = open(SERVER_PID_FILE,O_CREAT|O_WRONLY, 000774);
    if(fd_fifo_game_server_pid == -1){
        perror("open");
        serv_exit(2, "Error while creating the pid file !\n");
    }
    pid_t pid = getpid();
    if(write(fd_fifo_game_server_pid, &pid, sizeof(pid_t)) == -1){
        perror("write");
        serv_exit(3, "Error while writing the pid in the file !\n");
    }
    close(fd_fifo_game_server_pid);
    if( ! checkIfFileExists("/tmp/game_server.fifo")){
        if(mkfifo("/tmp/game_server.fifo",  S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) != 0){
            perror("mkfifo");
            serv_exit(4, "Error while creating the pipe /tmp/game_server.fifo\n");
        }
    }

    DIR *dir = opendir("/tmp/game_server");
    if (dir) {
        closedir(dir);
    } else if (ENOENT == errno) {
        if (mkdir("/tmp/game_server", 0700) != 0) {
            serv_exit(5, "Error while creating the directory /tmp/game_server\n");
        }
    } else {
        perror("opendir");
        serv_exit(6, "Error while opening the directory /tmp/game_server\n");
    }
    struct sigaction act;
    act.sa_handler = handSIGUSR1;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if(sigaction(SIGUSR1, &act, NULL) == -1){
        perror("sigaction");
        serv_exit(7, "Error while setting the sigrecv_stringnal handler\n");
    }
    int fd_fifo;
    size_t size_path;
    pid_t pid_game;
    char *gameName;
    char *gamePath;
    char **argv_game;
    char *pathPipe0;
    char *pathPipe1;
    while(1){
        if(usr1_receive){
            fd_fifo = open(GAME_FIFO, O_RDONLY);
            if (fd_fifo == -1) {
                perror("open");
                serv_exit(8,  "Error while opening the pipe for the communication between server and client !\n");
            }
            if(read(fd_fifo, &pid_client, sizeof(int)) == -1){
                fprintf(stderr, "Error while reading the pid of the client\n");
                perror("read");
                usr1_receive = 0;
                continue;
            }
            gameName = recv_string(fd_fifo);
            if(gameName == NULL){
                fprintf(stderr, "Error while receiving the game name\n");
                usr1_receive = 0;
                continue;
            }

            size_path = strlen(PATH_GAMES_OUT) + strlen(gameName) + strlen("_serv");
            gamePath = malloc(sizeof(char) * (size_path +1));
            strcpy(gamePath, PATH_GAMES_OUT);
            strcat(gamePath, gameName);
            strcat(gamePath, "_serv");
            gamePath[size_path] = '\0';
            if ( access(gamePath, F_OK) != 0 || access(gamePath, X_OK) != 0){
                fprintf(stderr, "The server can't find the game or it is not executable\n");
                free(gameName);
                free(gamePath);
                if(kill(pid_client, SIGUSR2) == -1){
                    perror("kill");
                    serv_exit(9, "Error while killing the client\n");
                }
                usr1_receive = 0;
                continue;
            }else{
                pid_game = fork();
                if(pid_game == -1){
                    perror("fork");
                    serv_exit(10, "Error while forking the game\n");
                }
                if (pid_game == 0) {
                    int nbArguments= 0;
                    if(read(fd_fifo, &nbArguments, sizeof(int)) == -1){
                        fprintf(stderr, "Error while reading the number of arguments\n");
                        perror("read");
                        usr1_receive = 0;
                        continue;
                    }
                    if(nbArguments != 0){
                        argv_game = recv_argv(fd_fifo);
                        char ** test = reallocarray(argv_game, nbArguments+2, sizeof(char*));
                        if(test == NULL){
                            perror("realloc");
                            serv_exit(11, "Error while reallocating the arguments\n");
                        }
                        // nbArguments = 3
                        // argv[0] = hangmann
                        // argv[1] = -n;
                        // argv[2] = 5; //-N
                        // argv[3] = NULL; // 5
                        // argv[4] = nonAlloced; //NULL

                        argv_game = test;
                        for(int k = nbArguments+1; k > 1; k--){
                            argv_game[k] = argv_game[k-1];
                        }
                        argv_game[1] = itoa(pid_client);
                        for(int l = 0 ; l < nbArguments +2; l++){
                            fprintf(stderr,"%s\n", argv_game[l]);
                        }
                        // for(int k = nbArguments+1; k > 0; k--){
                        //     argv_game[k] = argv_game[k-1];
                        // }
                        // argv_game[0] = itoa(pid_client);
                        if(argv_game == NULL){
                            fprintf(stderr, "Error while receiving the arguments of the game\n");
                            usr1_receive = 0;
                            free(gameName);
                            free(gamePath);
                            continue;
                        }
                    }else{
                        argv_game = NULL;
                    }
                    pathPipe0 = getPathFIFO(pid_client, 0);
                    pathPipe1 = getPathFIFO(pid_client, 1);
                    if(mkfifo(pathPipe0, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP | S_IROTH) != 0){
                        perror("mkfifo");
                        serv_exit(11, "Error while creating the pipes\n");
                    }
                    if(mkfifo(pathPipe1, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP | S_IROTH) != 0){
                        perror("mkfifo");
                        serv_exit(11, "Error while creating the pipes\n");
                    }
                    if(kill(pid_client, SIGUSR1) == -1){
                        perror("kill");
                        serv_exit(12, "Error while send signal to the client\n");
                    }
                    int fd_0 = open(pathPipe0, O_RDONLY);
                    if (fd_0 == -1) {
                        perror("open");
                        serv_exit(12, "Error while opening the pipe\n");
                    }
                    int fd_1 = open(pathPipe1, O_WRONLY);
                    if (fd_1 == -1) {
                        perror("open");
                        serv_exit(12, "Error while opening the pipe\n");
                    }
                    if(dup2(fd_0, 0) == -1){
                        perror("dup2");
                        serv_exit(13, "Error while duplicating the pipe 0 to stdout\n");
                    }
                    if(dup2(fd_1, 1) == -1){
                        perror("dup2");
                        serv_exit(13, "Error while duplicating the pipe 0 to stdout\n");
                    }
                    close(fd_0);
                    close(fd_1);
                    execv(gamePath, argv_game);
                    //execlp("valgrind", "valgrind", "-s","--leak-check=full", "--show-leak-kinds=all", gamePath, "-n","5",NULL);
                    perror("execv");
                    usr1_receive = 0;
                    free(gameName);
                    free(gamePath);
                    unlink(pathPipe0);
                    unlink(pathPipe1);
                    continue;
                }
            }
            free(gameName);
            free(gamePath);
            usr1_receive = 0;    
        }
    }
    return 0;
}