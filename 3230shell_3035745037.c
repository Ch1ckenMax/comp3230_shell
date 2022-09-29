//Student Name: Li Hoi Kit
//Student No.: 3035745037
//Development Platform: Windows WSL Ubuntu 
//Completion state: 


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

//Prints shell message and gets input from the user. Changes the arguments array and returns the number of arguments.
int getInput(char* arguments[], char input[]){
    //Initialize the arguments to null
    for(int i = 0; i < 30; i++){
        arguments[i] = NULL;
    }

    //Get input from the user
    int argumentCount = 0;
    printf("$$ 3230shell ## ");
    fgets(input, 1025, stdin);
    input[strcspn(input,"\n")] = 0;//Remove newline character at the end of string

    //Break input into separate arguments
    arguments[argumentCount] = strtok(input," ");
    while(arguments[argumentCount] != NULL && argumentCount < 30){
        argumentCount++;
        arguments[argumentCount] = strtok(NULL , " ");
    }

    return argumentCount;
}

//Returns the type of path. Returns 0 if path is in the environment variables or if the input length makes no sense, returns 1 otherwise (absolute path/relative path)
int pathType(char* path){
    if(strlen(path) == 0) return 0; //Makes no sense
    if(path[0] == '/' || path[0] == '.') return 1; //Absolute path
    return 0; //Environment variables
}

void runProgram(int argumentCount, char* arguments[]){
    //Fork and run
    pid_t childPID = fork();
    if(childPID < 0){ //Creation failure
        printf("%s\n", "Process creation failed!");
    }
    else if(childPID > 0){ //Parent process: the shell
        int childStatus;
        wait(&childStatus);
    }
    else{ //Child process
        if(pathType(arguments[0])){ //absolute/relative path
            execv(arguments[0], arguments);
        }else{ //environment variables
            execvp(arguments[0], arguments);
        }
        //Error! terminate the process NOT DONE YET
        perror("3230Shell");
        exit(0); //what exit state?
    }

}

int main(){
    //Initialize variables
    char input[1025]; //1024 character + termination character \0
    char* arguments[30];
    
    //Main loop body
    while(1){
        int argumentCount = getInput(arguments, input);
        runProgram(argumentCount, arguments);

        //Debug
        //printf("Path type:%d\n", pathType(arguments[0]));

        //printf("Arguments:\n");
        //printf("Argument count: %d\n", argumentCount);
        //for(int i = 0; i < argumentCount; i++){
        //    printf("%d: %s\n",i,arguments[i]);
        //}
    }

    return 0;
}