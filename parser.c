#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXCOM 2000
#define MAXLIST 30
#define MAXTOKEN 30

#define READ_END 0
#define WRITE_END 1

#define REGULAR_CMD 0
#define PIPED_CMD 1
#define OUT_RED_CMD 2
#define IN_RED_CMD 3

int getInput(char* str)
{
    char* buf;
    buf = readline("\n# ");
    if (strlen(buf) != 0) {
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}

char *strstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}

void parseWords(char* str, char** parsed1)
{
    int i;
    // printf("Str: %s", str);
    for (i = 0; i < MAXLIST; i++) {
        // printf("%s-", parsed1[i]);
        parsed1[i] = strsep(&str, " ");
        // printf("%s!", parsed1[i]);
        if (parsed1[i] == NULL)
            break;
        if (strlen(parsed1[i]) == 0)
            i--;
    }
}

void parsePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        if (strchr(str, '|') != NULL) {
            strpiped[i] = strsep(&str, "|"); // strsep can only divide with a char
        } else {
            strpiped[i] = str;
        }
        strpiped[i] = strstrip(strpiped[i]);
        // printf("~%s~\n", strpiped[i]);
        if (strpiped[i] == NULL)
            break;
    }

    // printf("%s\n", strpiped[0]);
    // printf("%s\n", strpiped[1]);
  
    // IF PIPED
    // return isPiped;
}

void parseOutRedirection(char* str, char** str_redirected)
{
    int i;
    for (i = 0; i < 2; i++) {
        if (strchr(str, '>') != NULL) {
            str_redirected[i] = strsep(&str, ">"); // strsep can only divide with a char
        } else {
            str_redirected[i] = str;
        }
        str_redirected[i] = strstrip(str_redirected[i]);
        // printf("~%s~\n", str_redirected[i]);
        if (str_redirected[i] == NULL)
            break;
    }
}

void parseInRedirection(char* str, char** str_redirected)
{
    int i;
    for (i = 0; i < 2; i++) {
        if (strchr(str, '<') != NULL) {
            str_redirected[i] = strsep(&str, "<"); // strsep can only divide with a char
        } else {
            str_redirected[i] = str;
        }
        str_redirected[i] = strstrip(str_redirected[i]);
        // printf("~%s~\n", str_redirected[i]);
        if (str_redirected[i] == NULL)
            break;
    }
}

int checkCmd(char* str)
{
    int i;
    int cmdType = REGULAR_CMD;
    if (strchr(str, '|'))
        cmdType = PIPED_CMD;
    if (strchr(str, '>'))
        cmdType = OUT_RED_CMD;
    if (strchr(str, '<'))
        cmdType = IN_RED_CMD;
    return cmdType;
}

void assignString(char* symbol, char* word, int length)
{
    for (size_t j = 0; j < length; j++)
    {
        symbol[j] = word[j];
    }
}

int checkSymbol(char* word)
{
    int isSymbol = 0;
    if (!strcmp(word, "|") || !strcmp(word, ">")  || !strcmp(word, "<") || !strcmp(word, "2>") )
    {
        isSymbol = 1;
    }
    return isSymbol;
}

void execArgs(char** parsed1)
{
    pid_t pid = fork();
    int result;
    // printf("%d\n", pid);
    if (pid == -1) {
        perror("\nFailed forking child..\n");
        return;
    } else if (pid == 0) { // Child
        if(execvp(parsed1[0], parsed1) < 0)
            perror("Could not execute command");
        exit(0);
    } else {
        wait(NULL);
    }
}

void execOutRedirection(char** cmd, char** output_f)
{
    int ofd;
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        // ofd = open(output_f, O_WRONLY|O_CREAT|O_TRUNC, 0644)
        ofd = creat(output_f[0], 0644); // Just one file
        dup2(ofd, STDOUT_FILENO);
        if (execlp(cmd[0], cmd[0], (char *)NULL)) // Just one command
            perror("Command could not be executed");
    }
    wait(NULL);
}

void execInRedirection(char** cmd, char** input_f)
{
    int ifd;
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        // ofd = creat(input_f[0], 0644); // Just one file
        ifd = open(input_f[0], O_RDONLY, 0644);
        dup2(ifd, STDIN_FILENO);
        if (execlp(cmd[0], cmd[0], (char *)NULL)) // Just one command
            perror("Command could not be executed");
    }
    wait(NULL);
}

void execErrRedirection(char** cmd, char** output_f)
{
    int ofd;
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        ofd = creat(output_f[0], 0644); // Just one file 
        dup2(ofd, STDERR_FILENO);
        execlp(cmd[0], cmd[0], (char *)NULL); // Just one command
    }
    wait(NULL);
}

void execArgsPiped(char** parsed1, char** parsed2)
{
    int pipefd[2];
    pid_t pid1, pid2;
    char *array;
    int status, done = 0;

    if (pipe(pipefd) < 0)
    {
        perror("\nPipe could not be initialized");
        return;
    }

    pid1 = fork();

    if (0 > pid1)
    {
        perror("Failed to create a child 1\n");
    }

    else if (0 == pid1)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);

        if (execlp(parsed1[0], parsed1[0], (char*) NULL)) {
            perror("\nCould not execute command 1..\n");
            exit(0);}
    }

    pid2 = fork();

    if (0 > pid2)
    {
        perror("Failed to create a child 2 \n");
    }

    else if(0 == pid2){

        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);

        if (execlp(parsed2[0], parsed2[0], (char *) NULL)) {
            perror("\nCould not execute command 2..\n");
            exit(0);}
    } 

    close(pipefd[0]);
    close(pipefd[1]);

    wait(NULL);
    wait(NULL);
}

