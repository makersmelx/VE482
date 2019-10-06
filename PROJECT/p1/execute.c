#include "shell.h"

void cpyArgsExec(int procIndex,int* pipeMark,char** args)
{
    
    int tmpArgNums = pipeMark[procIndex + 1] - pipeMark[procIndex];
    char **tmpArgs = malloc(sizeof(char *) * (tmpArgNums + 1));
    tmpArgs[tmpArgNums] = NULL;
    for (int i = 0; i < tmpArgNums;i++)
    {
        tmpArgs[i] = malloc(sizeof(char) * MAXLINE);
        strcpy(tmpArgs[i], args[pipeMark[procIndex] + i]);
    }
    execvp(tmpArgs[0], tmpArgs);
    errorPrompt();
    free(tmpArgs);
}

void execute(char **args, int argNum)
{
    int tmpLoc = 0;
    if(!strcmp(args[0],"cd"))
    {
        myCd(args);
        return;
    }
    else
    {
        pid_t pid = fork();
        if(pid<0)
        {
            errorPrompt();
            return;
        }
        addToAllFork(pid);
        if(pid == 0)
        {
            if(builtinCommand(args) == 0)
            {
                args[argNum] = NULL;
                execvp(args[0],args);
            }
            exit(0);
        }
        else
        {
            waitpid(pid,&tmpLoc,WUNTRACED);
        }
    } 
}
