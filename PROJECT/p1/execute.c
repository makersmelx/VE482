#include "shell.h"

void execute(char **args, int argNum)
{
    //args[argNum] = (char *)malloc(sizeof(char*));
    args[argNum] = NULL;
    execvp(args[0],args);
}
