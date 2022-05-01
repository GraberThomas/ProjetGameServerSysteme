#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "message.h"

int main(int argc, char **argv){
    if (mkfifo("testlibmessage_pipe.fifo",  S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) != 0){
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    int fd = open("testlibmessage_pipe.fifo", O_WRONLY);
    send_string(fd, argv[1]);
    send_argv(fd, argv+2);

    close(fd);
}