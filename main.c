#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "parser.h"

#define MAXCOM 1000
#define MAXLIST 100

void say_hi()
{
    char* username = getenv("USER");
    printf("\nHi %s, welcome to my Shell!\n", username);
}

int main(int argc, char **argv)
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    say_hi();

    while(1) {
        if (getInput(inputString))
            continue;
        execFlag = processString(inputString, parsedArgs, parsedArgsPiped);
        if (execFlag == 1)
            execArgs(parsedArgs);
        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}