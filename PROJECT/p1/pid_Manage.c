#include "shell.h"

void addToAllFork(int pidNum)
{
    allFork->forkPid[allFork->forkNum] = pidNum;
    allFork->forkNum++;
}

void resetFork()
{
    for(int i = 0; i< allFork->forkNum;i++)
    {
        kill(allFork->forkPid[i],SIGINT);
    }
    allFork->forkNum = 0;
    allFork->sigStatus = 0;
}







