#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../libs/libmessage/message.h"

int main(int argc, char **argv){
    for(int i = 0; i < argc; ++i){
        fprintf(stderr, "argv[%d]  = %s\n", i, argv[i]);
    }
    return 0;
} 