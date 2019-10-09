#include "shell.h"

int builtinCommand(char** args)
{
    if (!strcmp(args[0],"pwd"))
    {
        myPwd();
        return 1;
    }
    else if (!strcmp(args[0],"jobs"))
    {
        for(int i = 0; i< *procCount;i++)
        {
            //if(waitpid(PROCTABLE[i].pid,NULL,WNOHANG) == 0)
            if(PROCTABLE[i].done == 0)
            {
                printf("[%d] running %s\n" ,i+1,PROCTABLE[i].name);
            }
            else
            {
                printf("[%d] done %s\n" ,i+1,PROCTABLE[i].name);
            }

        }
        return  1;
    }
    else
    {
        return 0;
    }

}

