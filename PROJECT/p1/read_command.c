#include "shell.h"
int read_command(char **args, int *argNum)
//will set input and output redirection
{
    char *buffer = malloc(sizeof(char) * (MAXLINE + 1));
    char end = '\0';
    int state = PROCESSING;
    do
    {
        scanf("%[<>]",buffer);
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

        char* space = malloc(sizeof(char)*(MAXLINE+1));
        scanf("%[ ]",space);
        free(space);
        scanf("%[^><\n ]", buffer);
        end = getc(stdin);
        if(end == '<' || end =='>')
        {
            ungetc(end,stdin);
        }

        if (!strcmp(buffer, "exit"))
        {
            free(buffer);
            return EXIT;
        }
        else if(buffer[0]!='\0')
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
