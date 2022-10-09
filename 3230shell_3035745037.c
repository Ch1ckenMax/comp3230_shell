//Student Name: Li Hoi Kit
//Student No.: 3035745037
//Development Platform: Windows WSL Ubuntu 
//Completion state: 


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>
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

//Creates a child and use exec according to the arguments, and handle timeX command behavior
void runProgram(int argumentCount, char* arguments[], int isTimeX){
    //Fork and run
    pid_t childPID = fork();
    if(childPID < 0){ //Creation failure
        printf("%s\n", "Process creation failed!");
    }
    else if(childPID > 0){ //Parent process: the shell
        int childStatus;
        if(isTimeX == 1){ //timeX command, check time used
            struct rusage checkTime;
            wait3(&childStatus, 0, &checkTime); //also gets the user&system time the child process used and store it to checkTime
            printf("(PID)%d  (CMD)%s    (user)%0.3f s  (sys)%0.3f s\n",childPID ,arguments[1] , (float) checkTime.ru_utime.tv_sec + checkTime.ru_utime.tv_usec/1000000.0, (float) checkTime.ru_stime.tv_sec + checkTime.ru_stime.tv_usec/1000000.0 ); //USE wait4?
        }else{
            wait(&childStatus);
        }

        if(WIFSIGNALED(childStatus)){ //Child terminated by a signal
            printf("%s\n", strsignal(WTERMSIG(childStatus)));
        }
    }
    else{ //Child process
        if(isTimeX == 1){//Omit the first argument
            arguments++;
        }

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

//Checks if the command is an timeX command. Return 0 if it is not, return 1 if it is, return 2 if it is an invalid timeX command
int isTimeX(int argumentCount, char* arguments[]){
    if(strcmp(arguments[0],"timeX") == 0){ //If the first argument is timeX
        if(argumentCount == 1) 
            return 2; //Incorrect exit command
        else 
            return 1; //With other arguments, valid timeX usage.
    }
    else
        return 0; //Not an TimeX command
}

void sigint_handler(int signo){
    
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

        //timeX command behavior
        int isTimeXCommand = isTimeX(argumentCount, arguments);
        if(isTimeXCommand == 2){
            printf("3230shell: \"timeX\" cannot be a standalone command\n");
            continue;
        }

        //Do a loop 
            runProgram(argumentCount, arguments, isTimeXCommand);
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