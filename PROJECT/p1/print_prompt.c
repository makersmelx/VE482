#include "shell.h"

void print_prompt()
{
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
