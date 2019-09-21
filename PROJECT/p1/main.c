#include "shell.h"

void process()
{
    while (1)
    {

        char **args = malloc(sizeof(char *) * (MAXARGS + 2));
        char *filePath = malloc(sizeof(char) * (MAXLINE + 1));
        int argNum = 0;
        int inputState = 0;
        int promptFlag = 0;
        int stdiFD = dup(0);
        int stdoFD = dup(1);
        do
        {
            if (inputState == PROCESSING)
            //if (inputState == 0 && 11) for pipe
            {
                inputState = read_command(args, &argNum);
                if (inputState == EXIT)
                {
                    print_prompt();
                    free(args);
                    free(filePath);
                    printf("exit\n");
                    exit(0);
                }
                if (inputState == END_OF_LINE && argNum == 0)
                {
                    break;
                }

                if (inputState == PROCESSING || inputState == END_OF_LINE)
                    //No direction, directly execute
                {
                    if(promptFlag == 0)
                    {
                        promptFlag = 1;
                        pid_t pid;
                        pid = fork();
                        if (pid < 0)
                        {
                            printf("Fork error.\n");
                        }
                        else if (pid == 0)
                        {
                            print_prompt();
                            exit(0);
                        }
                        else
                        {
                            wait(NULL);
                        }
                    }

                    pid_t pid;
                    pid = fork();
                    if (pid < 0)
                    {
                        printf("Fork error.\n");
                    }
                    else if (pid == 0)
                    {

                        execute(args, argNum);
                        exit(0);
                    }
                    else
                    {
                        wait(NULL);
                    }
                }
                else
                    //redirection situation
                {
                    continue;
                }
            }
            else
            {


                if (promptFlag == 0)
                {
                    promptFlag = 1;
                    pid_t pid = fork();

                    if (pid < 0) {
                        printf("Fork error.\n");
                    } else if (pid == 0) {
                        print_prompt();
                        exit(0);
                    } else {
                        wait(NULL);
                    }
                }
                inputState = redirection(inputState, filePath);
                if (inputState == END_OF_LINE || inputState == PIPE)
                {

                    pid_t pid = fork();

                    if (pid < 0)
                    {
                        printf("Fork error.\n");
                    }
                    else if (pid == 0)
                    {
                        execute(args, argNum);
                        exit(0);
                    }
                    else
                    {
                        wait(NULL);
                    }
                }

            }

        } while (inputState != END_OF_LINE);

        for (int i = 0; i < MAXARGS + 2 && i != argNum; i++)
        {
            free(args[i]);
        }
        free(args);
        free(filePath);
        fflush(stdin);
        fflush(stdout);
        dup2(stdiFD, 0);
        dup2(stdoFD, 1);
        close(stdiFD);
        close(stdoFD);
    }
}

int main()
{
    process();
}
