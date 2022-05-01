#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "message.h"

int main(int argc, char **argv){
    int fd = open("testlibmessage_pipe.fifo", O_RDONLY);
    char *nomJeu = recv_string(fd);
    fprintf(stdout, "Nom du jeu : %s\n", nomJeu);
    free(nomJeu);
    char **table = recv_argv(fd);
    char **oldTable = table;
    int i = 0;
    while(*table != NULL){
        printf("argument %d : %s\n", i, *table);
        free(*table);
        i++;
        table++;
    }
    free(oldTable);
    close(fd);
    unlink("testlibmessage_pipe.fifo");
}