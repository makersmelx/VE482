#include "shell.h"

#ifdef DEBUG
static void checkAllFork()
{
    printf("forkNum: %d\n", allFork->forkNum);
}

void pipeTest(int x,int stdIn,int stdOut)
{
    int tmpIn = dup(0);
    int tmpOut = dup(1);
    dup2(stdIn, 1);
    dup2(stdOut, 0);

    if (x != -1)
    {
        printf("!::%d ", x);
    }
    printf("Fish!\n");
    dup2(tmpOut, 1);
    dup2(tmpIn, 0);
}
#endif
int myPipe(int** pipeFd,int* pipeMark,char** args,int pipeNum)
{
    int procIndex = 0;
    int tmpLoc = 0;
    pid_t pid = 0,wpid = 0;
    for (int pipeIndex = 0; pipeIndex < pipeNum;pipeIndex++)
    {
        int pipeTmp = pipe(pipeFd[pipeIndex]);
        if (pipeTmp < 0)
        {
            for (int i = 0; i < pipeIndex; i++)
            {
                close(pipeFd[i][0]);
                close(pipeFd[i][1]);
            }
            errorPrompt();
            return PIPEERR;
        }
    }
    int procNum = pipeNum + 1;
    for (procIndex = 0; procIndex < procNum; procIndex++)
    {
        if ((pid =fork())== 0)
        {
            addToAllFork(pid);
#ifdef DEBUG
            checkAllFork();
#endif
            break;
        }
        // else if (pid < 0)
        // {
        //     errorPrompt();
        //     for (int i = 0; i < pipeNum; i++)
        //     {
        //         close(pipeFd[i][0]);
        //         close(pipeFd[i][1]);
        //     }
        //     return FORKERR;
        // }
    }
    if(procIndex == 0)
    {
#ifdef DEBUG
        printf("child pid = %d,ppid = %d\n",getpid(),getppid());
#endif
        for (int i = 0; i < pipeNum; i++)
        {
            if(i != procIndex)
            {
                close(pipeFd[i][0]);
                close(pipeFd[i][1]);
            }
        }
        close(pipeFd[procIndex][0]);
        dup2(pipeFd[procIndex][1], STDOUT_FILENO);
        cpyArgsExec(procIndex, pipeMark, args);
    }
    else if (procIndex == procNum - 1)
    {
#ifdef DEBUG
        printf("child pid = %d,ppid = %d\n",getpid(),getppid());
#endif
        for (int i = 0; i < pipeNum; i++)
        {
            if(i == procIndex - 1)
            {
                continue;
            }
            else
            {
                close(pipeFd[i][0]);
                close(pipeFd[i][1]);
            }
        }
        close(pipeFd[procIndex - 1][1]);
        dup2(pipeFd[procIndex - 1][0], 0);
        cpyArgsExec(procIndex, pipeMark, args);
        
    }
    else if (procNum > 2 && procIndex > 0 && procIndex < procNum-1)
    {
#ifdef DEBUG
        printf("child pid = %d,ppid = %d\n",getpid(),getppid());
#endif
        for (int i = 0; i < pipeNum; i++)
        {
            if(i == procIndex || i == procIndex -1)
            {
                continue;
            }
            else
            {
                close(pipeFd[i][0]);
                close(pipeFd[i][1]);
            }
        }
        close(pipeFd[procIndex - 1][1]);
        close(pipeFd[procIndex][0]);
        dup2(pipeFd[procIndex - 1][0], 0);
        dup2(pipeFd[procIndex][1], 1);
        cpyArgsExec(procIndex, pipeMark, args);
        

    }
    else
    {
        for (int i = 0; i < pipeNum; i++)
        {
            close(pipeFd[i][0]);
            close(pipeFd[i][1]);
        }
        while((wpid = waitpid(-1,&tmpLoc,WNOHANG)) != -1)
        {
            if(wpid > 0)
            {
                tmpLoc = 0;
            }
        }
    }
    return PIPE_END_OF_LINE;
}
