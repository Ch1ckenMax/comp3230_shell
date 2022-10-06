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

//Creates a child and use exec according to the arguments
void runProgram(int argumentCount, char* arguments[]){
    //Fork and run
    pid_t childPID = fork();
    if(childPID < 0){ //Creation failure
        printf("%s\n", "Process creation failed!");
    }
    else if(childPID > 0){ //Parent process: the shell
        int childStatus;
        wait(&childStatus);
        if(WIFSIGNALED(childStatus)){ //Child terminated by a signal
            printf("%s\n", strsignal(WTERMSIG(childStatus))); //UNFINISHED
        }
    }
    else{ //Child process
        if(pathType(arguments[0])){ //absolute/relative path
            execv(arguments[0], arguments);
        }else{ //environment variables
            execvp(arguments[0], arguments);
        }
        //Error! terminate the process NOT DONE YET
        printf("3230Shell: %s\n", strerror(errno));
        exit(0); //what exit state?
    }
}

//Separate commands according to "|" piping in arguments
void commandSeparationForPipe(int argumentCount, char* arguments[]){

}

//Checks if the command is an exit command. Return 0 if it is not, return 1 if it is, return 2 if it is an invalid exit command
int isExit(int argumentCount, char* arguments[]){
    if(strcmp(arguments[0],"exit") == 0){ //If the first argument is exit
        if(argumentCount == 1) 
            return 1; //Correct exit command
        else 
            return 2; //With arguments. Incorrect exit command
    }
    else
        return 0; //Not an exit command
}

int checkTimerX(int argumentCount, char* arguments[]){
    return 0;
}


int main(){
    //Initialize variables
    char input[1025]; //1024 character + termination character \0
    char* arguments[30];
    
    //Main loop body
    while(1){
        //Get input from user
        int argumentCount = getInput(arguments, input);

        //Exit command behavior
        int isExitCommand = isExit(argumentCount, arguments); 
        if(isExitCommand == 1){
            printf("3230shell: Terminated\n");
            exit(0);
        }
        else if(isExitCommand == 2){
            printf("3230shell: \"exit\" with other arguments!!!\n");
            continue;
        }

        //Do a loop 
            runProgram(argumentCount, arguments);
            //pipe data exchange

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