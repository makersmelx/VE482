#include "shell.h"

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
