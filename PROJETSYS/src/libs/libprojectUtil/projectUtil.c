#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "projectUtil.h"

char *itoa(int i){
    char *str = malloc(sizeof(char) * 11);
    sprintf(str, "%d", i);
    return str;
}

char *getPathFIFO(int pid, int id){
    size_t size = strlen(PATH_FIFO)+strlen("cli")+ strlen(itoa(pid))+strlen("_0")+ strlen(".fifo");
    char *path = malloc(sizeof(char) * (size+1));
    strcpy(path, PATH_FIFO);
    strcat(path, "cli");
    strcat(path, itoa(pid));
    strcat(path, "_");
    strcat(path, itoa(id));
    strcat(path, ".fifo");
    path[size] = '\0';
    return path;
}