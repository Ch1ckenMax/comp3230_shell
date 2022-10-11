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
int getInput(char* arguments[]){
    char input[1025]; //char array for fgets. 1024 character + termination character \0
    
    //Initialize the arguments to null, so that exec function knwos where the arguments end
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

//Creates a child and use exec according to the arguments, and handle timeX command behavior. 
//Write output of program to pipeBuffer for the next program/output to terminal.
//pipeStart, pipeEnd indicates the position of the program in the piped command. 
void runProgram(char* arguments[], int isTimeX, char pipeBuffer[], int* bufferSize, int pipeStart, int pipeEnd){

    //Create pipe
    int pipeC2PFileDes[2]; //communication from child to parent. used to retrieve the stdout of child.
    int pipeP2CFileDes[2]; //communication from parent to child. used to redirect the previous command's stdout to current command's stdin.
    pipe(pipeC2PFileDes);
    pipe(pipeP2CFileDes);

    //Add a signal mask before forking to prevent the SIGUSR1 of parent from being handled before child gets to sigwait for the signal
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    //Fork and run
    pid_t childPID = fork();
    if(childPID < 0){ //Creation failure
        printf("%s\n", "Process creation failed!");
        return;
    }
    else if(childPID > 0){ //Parent process: the shell
        int childStatus;

        close(pipeC2PFileDes[1]); //close pipe write for child to parent
        close(pipeP2CFileDes[0]); //close pipe read for parent to child
        if(write(pipeP2CFileDes[1], pipeBuffer, *bufferSize) == -1){
            printf("3230Shell: %s\n", strerror(errno));
        };
        kill(childPID, SIGUSR1);//ready to go. Send SIGUSR1 to child.

        if(isTimeX == 1){ //timeX command, check time used
            struct rusage checkTime;
            wait3(&childStatus, 0, &checkTime); //also gets the user&system time the child process used and store it to checkTime
            printf("(PID)%d  (CMD)%s    (user)%0.3f s  (sys)%0.3f s\n",childPID ,arguments[0] , (float) checkTime.ru_utime.tv_sec + checkTime.ru_utime.tv_usec/1000000.0, (float) checkTime.ru_stime.tv_sec + checkTime.ru_stime.tv_usec/1000000.0 ); //USE wait4?
        }else{
            wait(&childStatus);
        }

        if(pipeEnd != 1){
            if(*bufferSize = read(pipeC2PFileDes[0], pipeBuffer, 4096) == -1){
                printf("3230Shell: %s\n", strerror(errno));
            }
        }

        if(WIFSIGNALED(childStatus)){ //Child terminated by a signal
            printf("%s\n", strsignal(WTERMSIG(childStatus)));
        }

        close(pipeC2PFileDes[0]); //all done. close them.
        close(pipeP2CFileDes[1]);
    }
    else{ //Child process
        int sig;

        //write buffer to stdin
        if(pipeStart != 1){
            dup2(pipeP2CFileDes[0], STDIN_FILENO); //connect stdin of child to pipe read
            close(pipeP2CFileDes[0]);
        }
        if(pipeEnd != 1){
            dup2(pipeC2PFileDes[1], STDOUT_FILENO); //connect stdout of child to pipe write
            close(pipeC2PFileDes[1]);
        }
        close(pipeP2CFileDes[1]); //Close unnecessary pipe ends
        close(pipeC2PFileDes[0]);
        
        sigwait(&mask, &sig); //Wait for SIGUSR1 signal from parent before proceeding
        if(pathType(arguments[0])){ //absolute/relative path
            execv(arguments[0], arguments);
        }else{ //environment variables
            execvp(arguments[0], arguments);
        }
        //Error! terminate the process
        printf("3230Shell: %s\n", strerror(errno));
        exit(0); //what exit state?
    }
}

//Separate commands according to "|" piping in arguments. Return -2 if starting/ending with "|", return -1 if there is two consecutive "|", or else return the number of programs. Alter programArgumentIndex to specify where the arguments of the program starts in arguments[]
int commandSeparationForPipe(int argumentCount, char* arguments[], int isTimeX, int programArgumentIndex[]){
    int offsetForTimeX = 0;
    if(isTimeX == 1){//Omit the first argument by adding an offset to the indexes below
            offsetForTimeX = 1;
    }

    //Check error cases for arguments starting with/ending with "|". This will make the logic below easier to handle.
    if(strcmp(arguments[0 + offsetForTimeX],"|") == 0 || strcmp(arguments[argumentCount - 1],"|") == 0)
        return -2;

    int programCount = 0;
    int argumentStart = 0 + offsetForTimeX;
    
    for(int i = 0 + offsetForTimeX; i < argumentCount; i++){
        if(strcmp(arguments[i],"|") == 0){
            if(i != 0 && arguments[i-1] == NULL) //consecutive "|"? (we turned all previous "|" with NULL)
                return -1;
            programArgumentIndex[programCount++] = argumentStart;
            arguments[i] = NULL;
            argumentStart = i + 1;
        }
    }

    programArgumentIndex[programCount++] = argumentStart;
    return programCount;
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

//Signal handler for SIGINT
void sigint_handler(int signo){
    //print prompt message
    printf("\n$$ 3230shell ## ");
    fflush(stdout);
}

void sigusr1_handler(int signo){
    //do nothing
}

int main(){
    //Set up signal handlers
    signal(SIGINT, sigint_handler);
    signal(SIGUSR1, sigusr1_handler);

    //Initialize variables for input
    int argumentCount;
    char* arguments[31]; //30 arguments + 1 NULL pointer at the end
    arguments[30] = NULL; //this shall always be NULL
    
    //Initialize variables for pipe
    int programCount;
    int programArgumentIndex[5];
    char pipeBuffer[4096]; //enough characters right?
    int bufferSize; //Indicates the buffer size to read by the child process to the stdin before exec, so that the content from stdout of the previous program in the pipe can be redirected to the current program
    
    //Main loop body
    while(1){

        //Get input from user
        argumentCount = getInput(arguments);
        if(argumentCount == 0) continue; //no input?

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

        int programCount = commandSeparationForPipe(argumentCount, arguments, isTimeXCommand, programArgumentIndex);
        if(programCount == -2){
            printf("3230shell: should not have | at the start or end of command\n");
            continue;
        }
        else if(programCount == -1){
            printf("3230shell: should not have two consecutive | in-between command\n");
            continue;
        }

        bufferSize = 0;
        //Do a loop 
        for(int i = 0; i < programCount; i++){
            int pipeStart = 0, pipeEnd = 0;
            if(i == 0) pipeStart = 1;
            if(i == programCount - 1) pipeEnd = 1;
            runProgram(&arguments[programArgumentIndex[i]], isTimeXCommand, pipeBuffer, &bufferSize, pipeStart, pipeEnd);
        }

            //runProgram(argumentCount, arguments, isTimeXCommand);
            //pipe data exchange

        //Debug
        //printf("Path type:%d\n", pathType(arguments[0]));

        //printf("Arguments:\n");
        //printf("Argument count: %d\n", argumentCount);
        //for(int i = 0; i < argumentCount; i++){
        //    printf("%d: %s\n",i,arguments[i]);
        //}
        //printf("\n");

        //printf("Num of programs: %d\n\n", programCount);
        //for(int i = 0; i < programCount; i++){
        //    printf("Program %d,\n", i);
        //    int trasverse = programArgumentIndex[i];
        //    while(arguments[trasverse] != NULL){
        //        printf("%d: Argument %s\n", trasverse, arguments[trasverse]);
        //       trasverse++;
        //    }
        //    printf("\n");
        //}
    }

    return 0;
}