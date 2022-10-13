#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>

int main(){
    char bufferFuck[2048];
    printf("HELL FUCK ASS\n");
    scanf("%s",bufferFuck);
    printf("HELLO this is here before!\n");
    return 0;
    //char buffer[10] = {'F','U','C','K','\0'};
    //write(STDIN_FILENO, buffer,5);
}