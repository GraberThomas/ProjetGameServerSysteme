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

volatile int usr1_receive = 0; //The variable used when the signal SIGUSR1 is received
int pid_client; //The pid of the client

/**
    * @brief Check if the given file exists
    * @param filename The file name
    * @return 1 if the file exists, 0 otherwise
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

//The handler for SIGUSR1, received when the client is executed
void handSIGUSR1(int sig) {
    usr1_receive = 1;
}

/** The handler for sigint, which blocks SIGCHLD signals, and waits for the termination of all executed threads,
 *  and displays their status
 */
void handSIGINT(int sig){
    kill(0, SIGINT); //kill all child processes
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

//handler for sigchld, which diplay the status of the terminated child
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
    //check if the server is already running
    if(checkIfFileExists(SERVER_PID_FILE)){
        fprintf(stderr, "The server is already running\n");
        exit(1);
    }
    //set up the sigCHLD handler
    struct sigaction actCHILD;
    actCHILD.sa_handler = handSIGCHLD;
    actCHILD.sa_flags = 0;
    sigemptyset(&actCHILD.sa_mask);
    if(sigaction(SIGCHLD, &actCHILD, NULL) == -1){
        fprintf(stderr, "Error setting SIGCHLD handler\n");
        perror("sigaction");
        exit(45);
    }
    //set up the sigINT, sigQUIT and sigTERM handlers
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
    //Open or create the server pid file
    int fd_fifo_game_server_pid = open(SERVER_PID_FILE,O_CREAT|O_WRONLY, 000774);
    if(fd_fifo_game_server_pid == -1){
        perror("open");
        serv_exit(2, "Error while creating the pid file !\n");
    }
    //write the pid of the server in the pid file
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
    //Try to open the folder game_server, and create it if it doesn't exist
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
    //Set up the signal handler for SIGUSR1
    struct sigaction act;
    act.sa_handler = handSIGUSR1;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if(sigaction(SIGUSR1, &act, NULL) == -1){
        perror("sigaction");
        serv_exit(7, "Error while setting the sigrecv_stringnal handler\n");
    }
    int fd_fifo; //The descriptor for the fifo between generic server and generic client
    size_t size_path; //The size of the path of the game
    pid_t pid_game; //The pid for the fork
    char *gameName; //The name of the game
    char *gamePath; //The path of the game
    char **argv_game; //The arguments for the game
    char *pathPipe0; //The path of the pipe 0
    char *pathPipe1; //The path of the pipe 1
    while(1){
        //If usr1 is received, the server must launch the game
        if(usr1_receive){
            //Open the fifo between the generic server and the generic client
            fd_fifo = open(GAME_FIFO, O_RDONLY);
            if (fd_fifo == -1) {
                perror("open");
                serv_exit(8,  "Error while opening the pipe for the communication between server and client !\n");
            }
            //Read the pid of the client
            if(read(fd_fifo, &pid_client, sizeof(int)) == -1){
                fprintf(stderr, "Error while reading the pid of the client\n");
                perror("read");
                usr1_receive = 0;
                continue;
            }
            //Read the name of the game
            gameName = recv_string(fd_fifo);
            if(gameName == NULL){
                fprintf(stderr, "Error while receiving the game name\n");
                usr1_receive = 0;
                continue;
            }
            //Make the game path
            size_path = strlen(PATH_GAMES_OUT) + strlen(gameName) + strlen("_serv");
            gamePath = malloc(sizeof(char) * (size_path +1));
            strcpy(gamePath, PATH_GAMES_OUT);
            strcat(gamePath, gameName);
            strcat(gamePath, "_serv");
            gamePath[size_path] = '\0';
            //Test if the game exists and if it is executable
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
            }
            //Create the child
            pid_game = fork();
            if(pid_game == -1){
                perror("fork");
                serv_exit(10, "Error while forking the game\n");
            }
            //The code for the child
            if (pid_game == 0) {
                //Read the number of arguments, send by the client
                int nbArguments= 0;
                if(read(fd_fifo, &nbArguments, sizeof(int)) == -1){
                    fprintf(stderr, "Error while reading the number of arguments\n");
                    perror("read");
                    usr1_receive = 0;
                    continue;
                }
                argv_game = recv_argv(fd_fifo);
                if(argv_game == NULL){
                    fprintf(stderr, "Error while receiving the arguments of the game\n");
                    usr1_receive = 0;
                    free(gameName);
                    free(gamePath);
                    continue;
                }
                //We will transmit the client's pid to the game server so that it can send signals to it
                // Just after the name of the game, in argv[1]. We have to realloc & shift the arguments.
                char ** test = reallocarray(argv_game, nbArguments+2, sizeof(char*));
                if(test == NULL){
                    perror("realloc");
                    serv_exit(11, "Error while reallocating the arguments\n");
                }
                argv_game = test;
                for(int k = nbArguments+1; k > 1; k--){
                    argv_game[k] = argv_game[k-1];
                }
                argv_game[1] = itoa(pid_client);
                //Create the pipes for communication between the game server and the game client
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
                //The pipes is created, send the signal to the client
                if(kill(pid_client, SIGUSR1) == -1){
                    perror("kill");
                    serv_exit(12, "Error while send signal to the client\n");
                }
                //Open and dup the pipes
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
                //execlp("valgrind", "valgrind", "-s","--leak-check=full", "--show-leak-kinds=all", gamePath,itoa(pid_client), "-n","5",NULL);
                perror("execv");
                usr1_receive = 0;
                free(gameName);
                free(gamePath);
                unlink(pathPipe0);
                unlink(pathPipe1);
                continue;
            }
            free(gameName);
            free(gamePath);
            usr1_receive = 0;    
        }
    }
    return 0;
}