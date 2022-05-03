#include <stdio.h>
#include <stdlib.h>

char *itoa(int i){
    char *str = malloc(sizeof(char) * 11);
    sprintf(str, "%d", i);
    return str;
}