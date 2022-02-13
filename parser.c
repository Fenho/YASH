#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#include <sys/wait.h>

#define MAXCOM 1000
#define MAXLIST 100

int getInput(char* str)
{
    char* buf;
    buf = readline("\n# ");
    // strcpy(str, buf);
    // return 0;
    // buf = readline("\n# ");
    if (strlen(buf) != 0) {
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}

void parseWords(char* str, char** parsed)
{
    int i;
  
    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");
        if (parsed[i] == NULL)
            break;
        // if (strlen(parsed[i]) == 0)
        //     i--;
    }
}

int parsePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }
  
    if (strpiped[1] == NULL)
        return 0;
    else {
        return 1;
    }
}

int processString(char* str, char** parsed, char** parsedpipe)
{
  
    char* strpiped[2];
    int piped = 0;
  
    piped = parsePipe(str, strpiped);
  
    if (piped) {
        parseWords(strpiped[0], parsed);
        parseWords(strpiped[1], parsedpipe);
    } else {
        parseWords(str, parsed);
    }

    return 1 + piped;
}

void execArgs(char** parsed)
{
    pid_t pid = fork();
    int result;

    if (pid == -1) {
        printf("\nFailed forking child..\n");
        return;
    } else if (pid == 0) { // Child
        result = execvp(parsed[0], parsed);
        if (result < 0) {
            printf("\nCould not execute command..\n");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);

        return;
    }
}

void execArgsPiped(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end
    int pipefd[2]; 
    pid_t p1, p2;
  
    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }
  
    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
  
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } else {
        // Parent executing
        p2 = fork();
  
        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        }
  
        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        } else {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}
