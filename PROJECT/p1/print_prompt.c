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
    signal(SIGCHLD,SIG_DFL);
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

void prompt_any(char* str)
{
    signal(SIGCHLD,SIG_DFL);
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        printf("Fork error.\n");
    }
    else if (pid == 0)
    {
        printf("%s",str);
        exit(0);
    }
    else
    {
        wait(NULL);
    }

}

void errorPrompt(int state,char* filePath)
{
    if(state < PIPE_CMD_NOT_FOUND)
    {
        prompt_any(filePath);
        prompt_any(": command not found\n");
        return;
    }
    switch(state)
    {
        case DUPLICATED_OUTPUT:
        {
            prompt_any("error: duplicated output redirection\n");
            break;
        }
        case DUPLICATED_INPUT:
        {
            prompt_any("error: duplicated input redirection\n");
            break;
        }
        case MISSING_PROGRAM:
        {
            prompt_any("error: missing program\n");
            break;
        }
        case PERMISSION_DENIED:
        {
            prompt_any(filePath);
            prompt_any(": Permission denied\n");
            break;
        }
        case SYNTAX_ERROR:
        {
            prompt_any("syntax error near unexpected token `");
            prompt_any(filePath);
            prompt_any("\'\n");
            break;
        }

        case NO_FILE_DIR_ERR:
        {
            prompt_any(filePath);
            prompt_any(": No such file or directory\n");
            break;
        }

        case COMMAND_NOT_FOUND:
        {
            prompt_any(filePath);
            prompt_any(": command not found\n");
            break;
        }

        default:
            break;
    }
}
