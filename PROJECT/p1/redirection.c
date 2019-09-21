#include "shell.h"

int redirection(int state, char *filePath)
{
    scanf("%[<>]",filePath);
    if (!strcmp(filePath,"<"))
    {
        return SINGLE_LEFT_REDIRECTION;
    }

    else if (!strcmp(filePath,">>"))
    {
        return DOUBLE_RIGHT_REDIRECTION;
    }
    else if (!strcmp(filePath,">"))
    {
        return SINGLE_RIGHT_REDIRECTION;
    }
    char* space = malloc(sizeof(char)*(MAXLINE+1));
    scanf("%[ ]",space);
    free(space);
    char end='\0';
    //scanf("%s",filePath);
    scanf("%[^><\n ]", filePath);
    end = getc(stdin);
    if(end == '<' || end =='>')
    {
        ungetc(end,stdin);
    }


    switch (state)
    {
    case SINGLE_LEFT_REDIRECTION:
    {
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

    
    if(end == '\n')
    {
        return END_OF_LINE;
    }
    else if (end == '|')
    {
        return PIPE;
    }
    return PROCESSING;
}
