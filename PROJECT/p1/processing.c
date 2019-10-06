#include "shell.h"
int process(int inputState,char** args,char*filePath,int *argNum,int pipeExist)
{
    if (inputState == PROCESSING)
    {
        inputState = read_command(args, argNum);
        if (inputState == EXIT)
        {
            //print_prompt(promptFlag);
            
            return EXIT;
        }
        if(inputState < 0)
        //Error
        {
            return inputState;
        }
        else if (inputState == END_OF_LINE && *argNum == 0)
            //Blank line
        {
            return END_OF_LINE;
        }
        else if(inputState == END_OF_LINE && pipeExist == 1)
        {
            return PIPE_END_OF_LINE;
        }
        else if ((inputState == PROCESSING && pipeExist == 0) || inputState == END_OF_LINE)
            //No direction, directly execute
        {
            //print_prompt(promptFlag);
            execute(args,*argNum);
            if(allFork->sigStatus == SIGINT )
            {
                return END_OF_LINE;
            }
        }
        else
            //redirection && PIPE situation
        {
            return inputState;
        }
    }
    else if (inputState == PIPE)
    {
        return inputState;
    }
    else
        //redirection situation
    {
        
        inputState = redirection(inputState, filePath);
        if (inputState == END_OF_LINE)
        {
            //print_prompt(promptFlag);
            execute(args,*argNum);
            if(allFork->sigStatus == SIGINT )
            {
                return END_OF_LINE;
            }
        }

    }
    return inputState;
}

void loopProcess()
{
    while (1)
    {
        char **args = malloc(sizeof(char *) * (MAXARGS + 2));
        char *filePath = malloc(sizeof(char) * (MAXLINE + 1));
        int argNum = 0;
        int inputState = PROCESSING;
        int *promptFlag = malloc(sizeof(int));
        *promptFlag = 0;
        int stdiFD = dup(0);
        int stdoFD = dup(1);
        print_prompt(promptFlag);
        int** pipeFd = malloc(sizeof(int*)*MAX_PIPE);
        for(int i = 0 ; i < MAX_PIPE;i++)
        {
            pipeFd[i] = malloc(sizeof(int)*2);
        }
        int pipeNum = 0;//Number of "|"
        int pipeExist = 0;
        int pipeMark[MAX_PIPE+1]={0};//marks the loc that stores each command
        while(inputState != END_OF_LINE && inputState != PIPE_END_OF_LINE)
        {
            do
            {
                inputState = process(inputState,args,filePath,&argNum,pipeExist);
                if(inputState < PROCESSING)
                    //ERROR occurs
                {
                    break;
                }
            } while (inputState != END_OF_LINE && inputState != PIPE_END_OF_LINE && inputState != EXIT && inputState != PIPE);
            if(inputState < PROCESSING)
            //error
            {
                printf("There is an error.\n");
                break;
            }
            else if(inputState == EXIT)
            {
                break;
            }
            else if (inputState == PIPE)
            {
                pipeExist = 1;
                char *space = malloc(sizeof(char) * MAXLINE);
                scanf("%[ ]", space);
                free(space);
                char end = getc(stdin);
                if(end == '\n')
                {
                    inputState = PARSEERR;
                    break;
                }
                else
                {
                    ungetc(end, stdin);
                    pipeNum++;
                    pipeMark[pipeNum] = argNum;
                    inputState = PROCESSING;
                }  
            }
            else if (inputState == PIPE_END_OF_LINE)
            {
                //print_prompt(promptFlag);
                pipeMark[pipeNum + 1] = argNum;
                inputState = myPipe(pipeFd, pipeMark, args, pipeNum);
            }
            if(inputState < PROCESSING)
            //error
            {
                printf("There is an error.\n");
                break;
            }
        }

        resetFork();
        for (int i = 0; i < MAXARGS + 2 && i != argNum; i++)
        {
            free(args[i]);
        }
        free(args);

        for(int i = 0 ; i < pipeNum;i++)
        {
            free(pipeFd[i]);
        }
        free(pipeFd);
        free(filePath);
        free(promptFlag);
        fflush(stdin);
        fflush(stdout);
        dup2(stdiFD, 0);
        dup2(stdoFD, 1);
        close(stdiFD);
        close(stdoFD);
        if(inputState == EXIT)
        {
            printExit();
            free(allFork);
            exit(0);
        }

    }
}

