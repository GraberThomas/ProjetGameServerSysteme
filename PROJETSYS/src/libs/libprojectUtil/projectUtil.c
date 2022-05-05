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

int isInt(char *str){
    int i = 0;
    while(str[i] != '\0'){
        if(str[i] < '0' || str[i] > '9'){
            return 0;
        }
        i++;
    }
    return 1;
}

//count the number of lines in a file
int countLines(char *path){
    int nbLines = 0;
    FILE *file = fopen(path, "r");
    if(file == NULL){
        fprintf(stderr, "Error while opening the file : %s\n", path);
        perror("fopen");
        return -1;
    }
    char *line = malloc(sizeof(char) * 28);
    while(!feof(file)){
        fgets(line, 28, file);
        nbLines++;
    }
    fclose(file);
    free(line);
    return nbLines;
}

//Return the word with the numLine in the file
char *getWordByNumLine(char *path, int numLine){
    FILE *file = fopen(path, "r");
    if(file == NULL){
        fprintf(stderr, "Error while opening the file : %s\n", path);
        perror("fopen");
        return NULL;
    }
    char *line = malloc(sizeof(char) * 28);
    int i = 1;
    while(!feof(file)){
        fgets(line, 28, file);
        if(i == numLine){
            fclose(file);
            return line;
        }
        i++;
    }
    fclose(file);
    free(line);
    return NULL;
}