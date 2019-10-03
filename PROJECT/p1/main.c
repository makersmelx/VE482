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
        //int pipeMark = 0; // locate the command that on the right of '|'
        do
        {
//            if(inputState == PIPE) //The left command has not executed yet
//            {
//
//            }
            if (inputState == PROCESSING)
            {
                inputState = read_command(args, &argNum);
                if (inputState == EXIT)
                {
                    print_prompt();
                    free(args);
                    free(filePath);
                    printf("exit\n");
                    free(allFork);
                    exit(0);
                }
                if (inputState == END_OF_LINE && argNum == 0)
                //Blank line
                {
                    break;
                }

                if (inputState == PROCESSING || inputState == END_OF_LINE)
                    //No direction, directly execute
                {
                    if(promptFlag == 0)
                    {
                        promptFlag = 1;
                        print_prompt();
                    }
                    execute(args,argNum);
                    if(allFork->sigStatus == SIGINT ||allFork->sigStatus == SIGTERM )
                    {
                        break;
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
                if(promptFlag == 0)
                {
                    promptFlag = 1;
                    print_prompt();
                }
                inputState = redirection(inputState, filePath);
                if (inputState == END_OF_LINE)
                {
                    execute(args,argNum);
                    if(allFork->sigStatus == SIGINT ||allFork->sigStatus == SIGTERM )
                    {
                        break;
                    }
                }

            }

        } while (inputState != END_OF_LINE);

        for (int i = 0; i < MAXARGS + 2 && i != argNum; i++)
        {
            free(args[i]);
        }
        resetFork();
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
    allFork = malloc(sizeof(fork_t));
    signal(SIGINT,sigHandler);
    resetFork();
    process();
}
