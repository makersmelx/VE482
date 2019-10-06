#include "shell.h"
void sigHandler_C()
{
    fflush(stdin);
    for(int i = 0; i< allFork->forkNum;i++)
    {
        kill(allFork->forkPid[i],SIGINT);
    }
    allFork->forkNum = 0;
    allFork->sigStatus= SIGINT;
}

