#include "shell.h"

void redirection_t(redirect_t *r)
{
    if(r->inUse == 0)
    {
        return;
    }
    if(r->in == SINGLE_LEFT_REDIRECTION)
    {

        int iFD = open(r->inFile, O_CREAT | O_RDONLY, 0666);
        if (iFD > 0)
        {
            dup2(iFD, STDIN_FILENO);
        }
        close(iFD);
    }
    if(r->out  == SINGLE_RIGHT_REDIRECTION)
    {

        int oFD = open(r->outFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (oFD > 0)
        {
            dup2(oFD, STDOUT_FILENO);
        }
        close(oFD);
    }
    else if (r->out == DOUBLE_RIGHT_REDIRECTION)
    {
        int oFD = open(r->outFile, O_WRONLY|O_CREAT | O_APPEND , 0666);
        if (oFD > 0)
        {
            dup2(oFD, STDOUT_FILENO);
        }
        close(oFD);
    }
}

int redirection(int state, char *filePath,int pipeExist,redirect_t* pipeRedirect,int procIndex,int* inSet,int* outSet)
{
    strclear(filePath,MAXLINE);
    char* space = malloc(sizeof(char)*(MAXLINE));
    strclear(space,MAXLINE);
    scanf("%[ ]",space);
    catToBGPrompt(space);
    free(space);
    char end='\0';
    char commaTmp = getc(stdin);
    while(1)
    {
        if(commaTmp == '\'' || commaTmp == '\"')
        {
            int exitState = commaHandle(commaTmp,filePath);
            if(exitState == END_OF_LINE)
            {
                return END_OF_LINE;
            }
        }
        else
        {
            ungetc(commaTmp,stdin);
            scanf("%[<>|]",filePath);
            if (!strcmp(filePath,"<") || !strcmp(filePath,">>") || !strcmp(filePath,">") ||!strcmp(filePath,"|"))
            {
                errorFlush();
                return SYNTAX_ERROR;
            }
            char* space = malloc(sizeof(char)*(MAXLINE+1));
            strclear(space,MAXLINE);
            if(scanf("%[ ]",space)==EOF)
            {
                if(*ctrlC == SIGINT)
                {
                    *ctrlC = 0;
                    printf("\n");
                }
                free(space);
                return END_OF_LINE;
            }
            catToBGPrompt(space);
            free(space);
            end = getc(stdin);
            if(end == '\n')
            {
                prompt_any("> ");
                *crossLine = 1;
            }
            else if(end != '\n')
            {
                ungetc(end,stdin);
            }
            scanf("%[^><\n|\'\"& ]", filePath);
            catToBGPrompt(filePath);
        }
        commaTmp = getc(stdin);
        if(commaTmp == '\'' || commaTmp == '\"')
        {
            continue;
        }
        else
        {
            ungetc(commaTmp,stdin);
            break;
        }
    }




    if(pipeExist == 0)
    {
        pipeRedirect[0].inUse = 1;
        if(*crossLine == 1 && tmpfp.inUse == 0)
        {
            if(state == SINGLE_RIGHT_REDIRECTION || state == DOUBLE_RIGHT_REDIRECTION)
            {
                strcpy(tmpfp.outFile,filePath);
                tmpfp.out = state;
                tmpfp.inUse = 1;
            }
            else if (state == SINGLE_LEFT_REDIRECTION)
            {
                strcpy(tmpfp.inFile,filePath);
                tmpfp.in = state;
                tmpfp.inUse = 1;
            }
        }
        else
        {
            switch (state)
            {
            case SINGLE_LEFT_REDIRECTION:
            {
                if(*inSet == 0)
                {
                    *inSet = 1;
                }
                else
                {
                    errorFlush();
                    return DUPLICATED_INPUT;
                }
                pipeRedirect[0].in= 1;
                strcpy(pipeRedirect[0].inFile, filePath);
                int iFD = open(filePath, O_RDONLY, 0666);
                if (iFD > 0)
                {
                    dup2(iFD, 0);
                }
                else
                {
                    if(errno == EPERM || errno == EROFS )
                    {
                        errorFlush();
                        return PERMISSION_DENIED;
                    }
                    else if (errno == ENOENT)
                    {
                        errorFlush();
                        return NO_FILE_DIR_ERR;
                    }

                }
                close(iFD);
                break;
            }
            case SINGLE_RIGHT_REDIRECTION:
            {
                if(*outSet == 0)
                {
                    *outSet = 1;
                }
                else
                {
                    return DUPLICATED_OUTPUT;
                }
                pipeRedirect[0].out= 1;
                strcpy(pipeRedirect[0].outFile, filePath);
                int oFD = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (oFD > 0)
                {
                    dup2(oFD, 1);
                }
                else
                {
                    if(errno == EPERM || errno == EROFS )
                    //FOR JOJ
                    {
                        return PERMISSION_DENIED;
                    }

                }
                //error cond to be filled
                close(oFD);
                break;
            }

            case DOUBLE_RIGHT_REDIRECTION:
            {
                if(*outSet == 0)
                {
                    *outSet = 1;
                }
                else
                {
                    if(errno == EPERM || errno == EROFS )
                    //FOR JOJ
                    {
                        return PERMISSION_DENIED;
                    }
                    return DUPLICATED_OUTPUT;
                }
                pipeRedirect[0].out= 1;
                strcpy(pipeRedirect[0].outFile, filePath);
                int oFD = open(filePath, O_WRONLY|O_CREAT | O_APPEND , 0666);
                if (oFD > 0)
                {
                    dup2(oFD, 1);
                }
                else
                {
                    if(errno == EPERM)
                    {
                        return PERMISSION_DENIED;
                    }

                }
                //error cond to be filled
                close(oFD);
                break;
            }

            default:
                break;
            }
        }
    }
    else
    {
        pipeRedirect[procIndex].inUse = 1;
        switch (state)
        {
        case SINGLE_LEFT_REDIRECTION:
        {
            pipeRedirect[procIndex].in = SINGLE_LEFT_REDIRECTION;
            strcpy(pipeRedirect[procIndex].inFile, filePath);
            break;
        }
        case SINGLE_RIGHT_REDIRECTION:
        {
            pipeRedirect[procIndex].out = SINGLE_RIGHT_REDIRECTION;
            strcpy(pipeRedirect[procIndex].outFile, filePath);
            break;
        }

        case DOUBLE_RIGHT_REDIRECTION:
        {
            pipeRedirect[procIndex].out = DOUBLE_RIGHT_REDIRECTION;
            strcpy(pipeRedirect[procIndex].outFile, filePath);
            break;
        }

        default:
            break;
        }
    }
    end = getc(stdin);
    if(end == '<' || end == '>' || end == '|' || end == '\'' || end == '\"')
    {
        char tmp = getc(stdin);
        if(tmp =='\n')
        {
            prompt_any("> ");
            *crossLine = 1;
        }
        else
        {
            ungetc(tmp,stdin);
        }
        ungetc(end,stdin);
    }
    else if (end == '&')
    {
        char tmp = getc(stdin);
        if(tmp == '\n')
        {
            catToBGPrompt(" &");
            *background = 1;
        }
        else
        {
            ungetc(tmp,stdin);
        }
    }
    else if (end == ' ')
    {
        strcat(backgroundPrompt," ");
    }
    if(end == '\n')
    {
        return END_OF_LINE;
    }
    return PROCESSING;
}
