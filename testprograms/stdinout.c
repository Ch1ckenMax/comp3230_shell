#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>

int main(){
    char buffer[10] = {'F','U','C','K','\0'};
    write(STDIN_FILENO, buffer,5);
}