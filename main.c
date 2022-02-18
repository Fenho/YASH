#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "parser.h"

#define MAXCOM 2000
#define MAXLIST 30
#define MAXTOKEN 30

#define REGULAR_CMD 0
#define PIPED_CMD 1
#define OUT_RED_CMD 2
#define IN_RED_CMD 3

void processString(char* str, char** parsed1, char** parsed2)
{

    // Other way to parse (Gets every word and then executes two by two or just one if a pair is not available)
    int i;
    parseWords(str, parsed1); // get parsed1 as an array of words
    char fullLine[MAXTOKEN][MAXLIST];
    int wordCounter = 0;
    int wordHasSymbol = 0;
    char* symbol;
    // char otherSymbol[2];
    char* newSymbol;
    char* firstCmd[MAXTOKEN];
    char* secondCmd[MAXTOKEN];
    int hasSecond = 0;
    int wordCounter2 = 0;
    for (i = 0; i < MAXLIST; i++) {
        if (parsed1[i] == NULL)
        {
            firstCmd[wordCounter] = parsed1[i];
            if (hasSecond)
            {
                secondCmd[wordCounter2] = parsed2[i];
            }
            if (!wordHasSymbol)
            {
                execArgs(firstCmd);
                wordCounter = 0;
            }
            else
            {
                // printf("%s", symbol);
                if (!strcmp(symbol, ">"))
                {
                    execOutRedirection(firstCmd, secondCmd);
                }
                else if (!strcmp(symbol, "<"))
                {
                    execInRedirection(firstCmd, secondCmd);
                }
                else if (!strcmp(symbol, "2>"))
                {
                    execErrRedirection(firstCmd, secondCmd);
                }
                else if (!strcmp(symbol, "|"))
                {
                    execArgsPiped(firstCmd, secondCmd);
                }
            }
            return;
        }
        int isSymbol = checkSymbol(parsed1[i]);
        // printf("%d", isSymbol);
        if (!isSymbol) // If Word is not a symbol
        {
            if (!wordHasSymbol)
            {
                firstCmd[wordCounter] = parsed1[i];
                wordCounter++;
            }
            else
            {
                secondCmd[wordCounter2] = parsed1[i];
                wordCounter2++;
            }
        }
        else
        {
            symbol = parsed1[i];
            wordHasSymbol = 1;
        }
        
    }

}

int main(int argc, char **argv)
{
    char inputString[MAXCOM];
    char *parsedCmds2[MAXTOKEN], *parsedCmds1[MAXTOKEN];
    int execFlag = 0;

    while(1) {
        if (getInput(inputString))
            continue;
        processString(inputString, parsedCmds1, parsedCmds2);
        // printf("%d", execFlag);
        // switch (execFlag)
        // {
        // case REGULAR_CMD:
        //     execArgs(parsedCmds1);
        //     break;
        
        // case PIPED_CMD:
        //     execArgsPiped(parsedCmds1, parsedCmds2);
        //     break;

        // case OUT_RED_CMD:
        //     execOutRedirection(parsedCmds1, parsedCmds2);
        //     break;

        // case IN_RED_CMD:
        //     execInRedirection(parsedCmds1, parsedCmds2);
        //     break;
        // }
    }
    return 0;
}