#include "shell.h"

void glbInit()
{
    ctrlC = malloc(sizeof(int));
    *ctrlC = 0;

    background = malloc(sizeof(int));
    *background = 0;

    procCount = malloc(sizeof(int));
    *procCount = 0;

    backgroundPrompt = malloc(sizeof(char)*MAXLINE);

    crossLine = malloc(sizeof(int));
    *crossLine = 0;

    tmpfp.in = 0;
    tmpfp.out = 0;
    tmpfp.inUse = 0;
    tmpfp.inFile = malloc(sizeof(char)*MAXLINE);
    tmpfp.outFile = malloc(sizeof(char)*MAXLINE);

}
void glbFree()
{
    free(ctrlC);
    free(background);
    free(procCount);
    free(backgroundPrompt);
    free(crossLine);
    free(tmpfp.inFile);
    free(tmpfp.outFile);
}

void errorFlush()
{
    char* errTmp = malloc(sizeof(char)*MAXLINE);
    char c = '\0';
    if((c = getchar())!= EOF )
    {
        scanf("%[^\n]",errTmp);
    }
    free(errTmp);
}


int process(int inputState,char** args,char*filePath,int *argNum,int pipeExist,redirect_t* pipeRedirect,int pipeNum,int* inSet,int* outSet)
{
    if (inputState == PROCESSING)
    {
        inputState = read_command(args, argNum);
        if (inputState == EXIT)
        {
            return EXIT;
        }
        if(inputState < 0)
        {
            return inputState;
        }
        else if (inputState == END_OF_LINE && *argNum == 0)
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
            if(tmpfp.inUse == 1)
            {
                redirection_t(&tmpfp);
            }
            inputState = execute(args,*argNum,inputState);
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
        
        inputState = redirection(inputState, filePath,pipeExist,pipeRedirect,pipeNum,inSet,outSet);
        if (inputState == END_OF_LINE && pipeExist == 0)
        {
            if(*crossLine)
            {
                redirection_t(&tmpfp);
            }
            inputState = execute(args,*argNum,inputState);
        }
        else if (inputState == END_OF_LINE && pipeExist == 1)
        {
            return PIPE_END_OF_LINE;
        }

    }
    return inputState;
}

void loopProcess()
{
    while (1)
    {
        int inputState = PROCESSING;
        int *promptFlag = malloc(sizeof(int));
        *promptFlag = (*background == 1)? 1:0;
        *ctrlC = 0;

        free(background);
        background = malloc(sizeof(int));
        *background = 0;
        *crossLine = 0;

        tmpfp.in = 0;
        tmpfp.out = 0;
        tmpfp.inUse = 0;
        strclear(backgroundPrompt,MAXLINE);

        char **args = malloc(sizeof(char *) * (MAXARGS + 2));
        int argNum = 0;

        int stdiFD = dup(0);
        int stdoFD = dup(1);
        char *filePath = malloc(sizeof(char) * (MAXLINE));

        int *inSet = malloc(sizeof(int));
        int *outSet = malloc(sizeof(int));
        *inSet = 0;
        *outSet = 0;
        
        redirect_t *pipeRedirect = malloc(sizeof(redirect_t)*MAX_PIPE);
        for(int i = 0 ; i< MAX_PIPE;i++)
        {
            pipeRedirect[i].in = 0;
            pipeRedirect[i].out = 0;
            pipeRedirect[i].inUse = 0;
            pipeRedirect[i].inFile = malloc(sizeof(char) * (MAXLINE));
            pipeRedirect[i].outFile = malloc(sizeof(char )*(MAXLINE));
        }

        int **pipeFd = malloc(sizeof(int *) * MAX_PIPE);
        for(int i = 0 ; i < MAX_PIPE;i++)
        {
            pipeFd[i] = malloc(sizeof(int)*2);
        }
        int pipeNum = 0;//Number of "|"
        int pipeExist = 0;
        int pipeMark[MAX_PIPE+1]={0};//marks the loc that stores each command



        print_prompt(promptFlag);

        while(inputState != END_OF_LINE && inputState != PIPE_END_OF_LINE)
        {
            do
            {
                inputState = process(inputState,args,filePath,&argNum,pipeExist,pipeRedirect,pipeNum,inSet,outSet);
                if(inputState < PROCESSING)
                    //ERROR occurs
                {
                    break;
                }
            } while (inputState != END_OF_LINE && inputState != PIPE_END_OF_LINE && inputState != EXIT && inputState != PIPE);
            if(inputState < PROCESSING)
            //error
            {

                break;
            }
            else if(inputState == EXIT)
            {
                break;
            }
            else if (inputState == PIPE)
            {
                if(argNum == pipeMark[pipeNum])
                {
                    inputState = MISSING_PROGRAM;
                    errorFlush();
                    break;
                }
                else if(pipeRedirect[pipeNum].in == 1 && pipeNum != 0)
                {
                    inputState = DUPLICATED_INPUT;
                    errorFlush();
                    break;
                }

                else if(pipeRedirect[pipeNum].out == 1)
                {
                    inputState = DUPLICATED_OUTPUT;
                    errorFlush();
                    break;
                }
                pipeExist = 1;
                char *space = malloc(sizeof(char) * MAXLINE);
                strclear(space,MAXLINE);
                scanf("%[ ]", space);
                strcat(backgroundPrompt,space);
                free(space);
                char end = getc(stdin);
                if(end == '\n')
                {
                    prompt_any("> ");
                }
                else
                {
                    ungetc(end, stdin);
                }
                pipeNum++;
                pipeMark[pipeNum] = argNum;
                inputState = PROCESSING;

            }
            else if (inputState == PIPE_END_OF_LINE)
            {
                if(pipeRedirect[pipeNum].in == 1 && pipeNum != 0)
                {
                    inputState = DUPLICATED_INPUT;
                    break;
                }
                pipeMark[pipeNum + 1] = argNum;
                inputState = myPipe(pipeFd, pipeMark, args, pipeNum,pipeRedirect);
            }
            if(inputState < PROCESSING)
            //error
            {
                break;
            }
        }



        for(int i = 0 ; i < MAX_PIPE;i++)
        {
            free(pipeFd[i]);
        }
        free(pipeFd);

        for(int i = 0 ; i<MAX_PIPE;i++)
        {
            free(pipeRedirect[i].inFile);
            free(pipeRedirect[i].outFile);
        }
        free(pipeRedirect);

        dup2(stdiFD, 0);
        dup2(stdoFD, 1);
        close(stdiFD);
        close(stdoFD);

        if(inputState < PROCESSING)
        {
            if(inputState == COMMAND_NOT_FOUND)
            {
                errorPrompt(inputState,args[0]);
            }
            else if(inputState < PIPE_CMD_NOT_FOUND)
            {
                errorPrompt(inputState,args[PIPE_CMD_NOT_FOUND-inputState]);
            }
            else
            {
                errorPrompt(inputState,filePath);
            }
            setbuf(stdin, NULL);
        }

        for (int i = 0; i < MAXARGS + 2 && i != argNum; i++)
        {
            free(args[i]);
        }
        free(args);
        free(filePath);
        free(promptFlag);
        free(inSet);
        free(outSet);

        if(inputState == EXIT)
        {
            printExit();
            return;
        }
    }
}

