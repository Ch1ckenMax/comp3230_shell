//Student Name: Li Hoi Kit
//Student No.: 3035745037
//Development Platform: Windows WSL Ubuntu 
//Completion state: 


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

    //Break input into separate arguments
    arguments[argumentCount] = strtok(input," ");
    while(arguments[argumentCount] != NULL && argumentCount < 30){
        argumentCount++;
        arguments[argumentCount] = strtok(NULL , " ");
    }

    return argumentCount;
}

int main(){
    //Initialize variables
    char input[1025]; //1024 character + termination character \0
    char* arguments[30];
    
    //Main loop body
    while(1){
        int argumentCount = getInput(arguments, input);

        


        //Debug
        printf("%d\n", (int) strlen(arguments[0]));
        printf("%d\n", argumentCount);
        for(int i = 0; i < argumentCount; i++){
            printf("%d: %s\n",i,arguments[i]);
        }
    }

    return 0;
}