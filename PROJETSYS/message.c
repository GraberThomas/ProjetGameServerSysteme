#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "message.h"


int send_string(int fd, char *str){
    if(! str ) return 3;
    int len = strlen(str);
    if(write(fd, &len, sizeof(int)) < 0){
        perror("write");
        return 1;
    }
    if(write(fd, str, len*sizeof(char)) < 0){
        perror("write");
        return 2;
    }
    return 0;
}

char *recv_string(int fd) {
    int len;
    char *str;
    if(read(fd, &len , sizeof(int)) == -1){
        perror("read");
        return NULL;
    }
    len++;   
    str = calloc(sizeof(char), len);   
    if(str == NULL){
        perror("calloc");
        return NULL;
    }
    if(read(fd, str , sizeof(char) * (len-1)) == -1){
        perror("read");
        return NULL;
    }
    str[len-1] = '\0';
    return str;
}

int send_argv(int fd, char *argv[]){
    int argc = 0;
    char **oldArgv = argv;
    while( *oldArgv != NULL){
        argc++;
        oldArgv+=1;
    }
    fprintf(stdout,"ARGC = %d\n", argc);
    if(argc == 0){
        return 4;
    }
    if(write(fd, &argc, sizeof(int)) < 0){
        perror("write");
        return 5;
    }
    int result;
    for(int i = 0 ; i < argc ; i++){
        result = send_string(fd, argv[i]);
        if(result != 0){
            return result;
        }
    }
    return 0;
}

char **recv_argv(int fd) {
    int len;
    if (read(fd, &len, sizeof(int)) == -1) {
        perror("read");
        return NULL;
    }
    len++;
    char **argv = calloc(sizeof(char *), len);
    if(argv == NULL){
        perror("calloc");
        return NULL;
    }
    for (int i = 0; i < len-1; i++) {
        argv[i] = recv_string(fd);
        if (argv[i] == NULL) {
            return NULL;
        }
    }
    argv[len-1] = NULL;
    return argv;
}