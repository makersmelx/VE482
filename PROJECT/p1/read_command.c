#include "shell.h"
int read_command(char **args, int *argNum)
//will set input and output redirection
{
    char *buffer = malloc(sizeof(char) * (MAXLINE));
    buffer[0]='\0';
    char end = '\0';
    int state = PROCESSING;
    do
    {
        if(scanf("%[ ]",buffer) == EOF)
        {
            state = allFork->sigStatus ==  SIGINT?END_OF_LINE:EXIT;
            if(allFork->sigStatus == SIGINT)
            {
                printf("\n");
            }
            break;
        }
        scanf("%[<>|]",buffer);
        if (!strcmp(buffer,"<"))
        {
            state = SINGLE_LEFT_REDIRECTION;
            break;
        }

        else if (!strcmp(buffer,">>"))
        {
            state  = DOUBLE_RIGHT_REDIRECTION;
            break;
        }
        else if (!strcmp(buffer,">"))
        {
            state =  SINGLE_RIGHT_REDIRECTION;
            break;
        }
        else if(!strcmp(buffer,"|"))
        {
            state = PIPE;
            break;
        }

        char* space = malloc(sizeof(char)*(MAXLINE+1));
        scanf("%[ ]",space);
        free(space);
        scanf("%[^><|\n ]", buffer);
        //Judge whether it reaches end of the line.
        end = getc(stdin);
        if(end == '<' || end == '>' || end == '|')
        {
            ungetc(end,stdin);
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
        }
    } while (end != '\n');
    if (end == '\n' && state == PROCESSING)
    {
        state = END_OF_LINE;
    }

    free(buffer);
    return state;
}
