#include "shell.h"

void print_prompt(int* promptFlag)
{
    if(*promptFlag == 0)
    {
        *promptFlag = 1;
        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            printf("Fork error.\n");
        }
        else if (pid == 0)
        {
            printf("mumsh $ ");
            exit(0);
        }
        else
        {
            wait(NULL);
        }
    }
}

void printExit()
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        printf("Fork error.\n");
    }
    else if (pid == 0)
    {
        printf("exit\n");
        exit(0);
    }
    else
    {
        wait(NULL);
    }
}

void errorPrompt()
{
    perror("Error: ");
}
