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

void handSIGUSR1(int sig) {
    write(1, "SIGUSR1 received\n", 18);
    usr1_receive = 1;
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

int main(int argc, char **argv){
    struct sigaction temp;
    sigemptyset(&temp.sa_mask);
    temp.sa_flags = 0;
    temp.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &temp, NULL);
    if(checkIfFileExists(SERVER_PID_FILE)){
        serv_exit(1, "Server already running !\n");
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
        serv_exit(7, "Error while setting the signal handler\n");
    }

    int pid_client;
    int fd_fifo;
    int childResult;
    size_t size_path;
    pid_t pid_game;
    char *gameName;
    char *gamePath;
    char **argv_game;
    while(1){
        if(usr1_receive){
            printf("Debut\n");
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

            size_path = strlen("./out/game/") + strlen(gameName) + strlen("_serv");
            gamePath = malloc(sizeof(char) * (size_path +1));
            strcpy(gamePath, "./out/game/");
            strcat(gamePath, gameName);
            strcat(gamePath, "_serv");
            gamePath[size_path] = '\0';
            printf("Game path : %s\n", gamePath);
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
                if (pid_game == -1) {
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
                    printf("game name : %s\n", gameName);
                    char *pathPipe0 = getPathFIFO(pid_client, 0);
                    char *pathPipe1 = getPathFIFO(pid_client, 1);
                    printf("pathPipe0 : %s\n", pathPipe0);
                    printf("pathPipe1 : %s\n", pathPipe1);
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
                    printf("j'ai envoye sigusr1\n");
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
                    printf(" fd_0 : %d, fd_1 : %d\n", fd_0, fd_1);
                    if(dup2(fd_0, 0) != 0){
                        fprintf(stderr, "Error while duplicate the pipe 0 to stdout\n");
                        perror("dup2");
                    }
                    int res2 = dup2(fd_1, 1);
                    if(res2 != 0){
                        fprintf(stderr, "%d\n", res2);
                        fprintf(stderr, "%d" , errno);
                        fprintf(stderr, "Error while duplicate the pipe 1 to stdout\n");
                        perror("dup2");
                    }
                    close(fd_0);
                    close(fd_1);
                    execv(gamePath, argv_game);
                    perror("execv");
                    usr1_receive = 0;
                    free(gameName);
                    free(gamePath);
                    continue;
                }
            }
            free(gameName);
            free(gamePath);
            wait(&childResult);
            if (WIFEXITED(childResult)) {
                printf("exited, status=%d\n", WEXITSTATUS(childResult));
            } else if (WIFSIGNALED(childResult)) {
                printf("killed by signal %d\n", WTERMSIG(childResult));
            } else if (WIFSTOPPED(childResult)) {
                printf("stopped by signal %d\n", WSTOPSIG(childResult));
            } else if (WIFCONTINUED(childResult)) {
                printf("continued\n");
            }
            usr1_receive = 0;
        }
    }
    return 0;
}