#include "shell.h"

void catToBGPrompt(char* str)
{

    strcat(backgroundPrompt,str);

}

void strclear(char* vic, int size)
{
    for(int i = 0; i< size;i++)
    {
        vic[i] = '\0';
    }
}

static int commaHandle_unit(char commas, char* buffer)
{
    char *scnf_tmp = malloc(sizeof(char) * MAXLINE);
    strclear(scnf_tmp,MAXLINE);
    while(1)
    {
        strclear(scnf_tmp,MAXLINE);
        if(commas == '\'')
        {
            if(scanf("%[^\'\n]",scnf_tmp) ==EOF)
            {
                if(*ctrlC == SIGINT)
                {
                    *ctrlC = 0;
                    printf("\n");
                }
                free(scnf_tmp);
                return END_OF_LINE;
            }

        }
        else if(commas == '\"')
        {
            if(scanf("%[^\"\n]",scnf_tmp)==EOF)
            {
                if(*ctrlC == SIGINT)
                {
                    *ctrlC = 0;
                    printf("\n");
                }
                free(scnf_tmp);
                return END_OF_LINE;
            }
        }
        strcat(buffer, scnf_tmp);
        char _comma = getc(stdin);
        if(_comma == commas)
        {
            break;
        }
        else if (_comma == '\n')
        {
            prompt_any("> ");
            *crossLine = 1;
            strclear(scnf_tmp,MAXLINE);
            scnf_tmp[0]='\n';
            scnf_tmp[1]='\0';
            strcat(buffer,scnf_tmp);
            strclear(scnf_tmp,MAXLINE);
        }
    }
    strcat(backgroundPrompt,buffer);
    free(scnf_tmp);
    return PROCESSING;
}

int commaHandle(char commaTmp, char*buffer)
{
    while(1)
    {
        commaHandle_unit(commaTmp,buffer);
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
    return PROCESSING;
}


int read_command(char **args, int *argNum)
//will set input and output redirection
{
    char *buffer = malloc(sizeof(char) * (MAXLINE));
    strclear(buffer,MAXLINE);
    buffer[0]='\0';
    char end = '\0';
    int state = PROCESSING;
    do
    {
        if(scanf("%[ ]",buffer) == EOF)
        {
            state = (*ctrlC ==  SIGINT)?END_OF_LINE:EXIT;
            if(*ctrlC == SIGINT)
            {
                *ctrlC = 0;
                printf("\n");
            }
            break;
        }
        catToBGPrompt(buffer);
        int breakScanWhile = 0;
        char commaTmp = getc(stdin);
        while(1)
        {
            if(commaTmp == '\'' || commaTmp == '\"')
            {
                commaHandle(commaTmp,buffer);
            }
            else
            {
                ungetc(commaTmp,stdin);
                scanf("%[<>|]",buffer);
                if (!strcmp(buffer,"<"))
                {
                    catToBGPrompt(buffer);
                    state = SINGLE_LEFT_REDIRECTION;
                    breakScanWhile = 1;
                    break;
                }
                else if (!strcmp(buffer,">>"))
                {
                    catToBGPrompt(buffer);
                    state  = DOUBLE_RIGHT_REDIRECTION;
                    breakScanWhile = 1;
                    break;
                }
                else if (!strcmp(buffer,">"))
                {
                    catToBGPrompt(buffer);
                    state =  SINGLE_RIGHT_REDIRECTION;
                    breakScanWhile = 1;
                    break;
                }
                else if(!strcmp(buffer,"|"))
                {
                    catToBGPrompt(buffer);
                    state = PIPE;
                    breakScanWhile = 1;
                    break;
                }

                char* space = malloc(sizeof(char)*(MAXLINE));
                strclear(space,MAXLINE);
                scanf("%[ ]",space);
                catToBGPrompt(space);
                scanf("%[^><|\n\'\"& ]", buffer);
                catToBGPrompt(buffer);
                free(space);
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
        if(breakScanWhile == 1)
        {
            break;
        }
            //Judge whether it reaches end of the line.
        end = getc(stdin);
        if(end == '<' || end == '>' || end == '|' || end == '\'' || end == '\"')
        {
            ungetc(end,stdin);
        }
        else if (end == '&')
        {
            end = getc(stdin);
            if(end == '\n')
            {
                catToBGPrompt(" &");
                *background = 1;
            }
            else
            {
                ungetc(end,stdin);
            }
        }
        else if(end == ' ')
        {
            strcat(backgroundPrompt," ");
        }



        if (!strcmp(buffer, "exit"))
        {
            if(*argNum < 1)
            {
                free(buffer);
                return EXIT;
            }

        }

        if(buffer[0]!='\0')
        {
            args[*argNum] = (char *)malloc(sizeof(char) * (strlen(buffer) + 1));
            strcpy(args[*argNum], buffer);
            (*argNum)++;
            strclear(buffer,MAXLINE);
        }
    } while (end != '\n' );
    if (end == '\n' && state == PROCESSING)
    {
        state = END_OF_LINE;
    }

    free(buffer);
    return state;
}
