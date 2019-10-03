#include "shell.h"

int builtinCommand(char** args)
{
    if (!strcmp(args[0],"pwd"))
    {
        myPwd();
        return 1;
    }
    else
    {
        return 0;
    }

}

