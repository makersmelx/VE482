#include "shell.h"

void redirection_t(redirect_t *r)
{
    if(r->inUse == 0)
    {
        return;
    }
    if(r->in == 1)
    {

        int iFD = open(r->inFile, O_CREAT | O_RDONLY, 0666);
        if (iFD > 0)
        {
            dup2(iFD, STDIN_FILENO);
        }
        //error cond to be filled
        close(iFD);
    }
    if(r->out  == 1)
    {

        int oFD = open(r->outFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (oFD > 0)
        {
            dup2(oFD, STDOUT_FILENO);
        }
        //error cond to be filled
        close(oFD);
    }
    else if (r->out == 2)
    {
        int oFD = open(r->outFile, O_WRONLY|O_CREAT | O_APPEND , 0666);
        if (oFD > 0)
        {
            dup2(oFD, STDOUT_FILENO);
        }
        close(oFD);
    }
}

int redirection(int state, char *filePath,int pipeExist,redirect_t* pipeRedirect,int procIndex)
{
    scanf("%[<>|]",filePath);
    if (!strcmp(filePath,"<") || !strcmp(filePath,">>") || !strcmp(filePath,">") ||!strcmp(filePath,"|"))
    {
        return PARSEERR;
    }
    char* space = malloc(sizeof(char)*(MAXLINE+1));
    scanf("%[ ]",space);
    free(space);
    char end='\0';
    scanf("%[^><\n| ]", filePath);
    end = getc(stdin);
    if(end == '<' || end =='>' || end == '|')
    {
        ungetc(end,stdin);
    }

    if(pipeExist == 0)
    {
        pipeRedirect[0].inUse = 1;
        switch (state)
        {
        case SINGLE_LEFT_REDIRECTION:
        {
            strcpy(pipeRedirect[0].inFile, filePath);
            int iFD = open(filePath, O_CREAT | O_RDONLY, 0666);
            if (iFD > 0)
            {
                dup2(iFD, 0);
            }
            //error cond to be filled
            close(iFD);
            break;
        }
        case SINGLE_RIGHT_REDIRECTION:
        {
            strcpy(pipeRedirect[0].outFile, filePath);
            int oFD = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (oFD > 0)
            {
                dup2(oFD, 1);
            }
            //error cond to be filled
            close(oFD);
            break;
        }

        case DOUBLE_RIGHT_REDIRECTION:
        {
            strcpy(pipeRedirect[0].outFile, filePath);
            int oFD = open(filePath, O_WRONLY|O_CREAT | O_APPEND , 0666);
            if (oFD > 0)
            {
                dup2(oFD, 1);
            }
            //error cond to be filled
            close(oFD);
            break;
        }

        default:
            break;
        }
    }
    else
    {
        pipeRedirect[procIndex].inUse = 1;
        switch (state)
        {
        case SINGLE_LEFT_REDIRECTION:
        {
            pipeRedirect[procIndex].in = 1;
            strcpy(pipeRedirect[procIndex].inFile, filePath);
            break;
        }
        case SINGLE_RIGHT_REDIRECTION:
        {
            pipeRedirect[procIndex].out = 1;
            strcpy(pipeRedirect[procIndex].outFile, filePath);
            break;
        }

        case DOUBLE_RIGHT_REDIRECTION:
        {
            pipeRedirect[procIndex].out = 2;
            strcpy(pipeRedirect[procIndex].outFile, filePath);
            break;
        }

        default:
            break;
        }
    }

    if(end == '\n')
    {
        return END_OF_LINE;
    }
    return PROCESSING;
}
