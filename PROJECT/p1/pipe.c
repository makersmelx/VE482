#include "shell.h"

#ifdef DEBUG
void pipeTest(redirect_t r,int stdIn,int stdOut)
{
    int tmpIn = dup(0);
    int tmpOut = dup(1);
    dup2(stdIn, 1);
    dup2(stdOut, 0);
    printf("Fish!\n");
    printf("in: %d", r.in);
    printf("out: %d", r.out);
    printf("inFile: %s", r.inFile);
    printf("outFile: %s", r.outFile);
    printf("inUse: %d", r.inUse);
    dup2(tmpOut, 1);
    dup2(tmpIn, 0);
}
#endif
int myPipe(int** pipeFd,int* pipeMark,char** args,int pipeNum,redirect_t* pipeRedirect)
{
    if(*background == 1)
    {
        signal(SIGCHLD,SIG_DFL);
        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            printf("Fork error.\n");
        }
        else if (pid == 0)
        {
            printf("[%d] %s\n",*procCount+1,backgroundPrompt);
            exit(0);
        }
        else
        {
            if(*background == 1)
            {
                prompt_any("mumsh $ ");
            }
            wait(NULL);
        }
    }
    int tmpIndex = 0;
    int procIndex = 0;
    int tmpLoc = 0;
    int count = 0;
    pid_t pid = 0,wpid = 0;
    // start a inter-proc communication
    int shmid = shmget(IPC_PRIVATE,MAXLINE,IPC_CREAT | 0600);
    struct shmid_ds buf;
    int* errorCode = NULL;
    int err = 0;

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
            return PIPEERR;
        }
    }
    int procNum = pipeNum + 1;
    for (procIndex = 0; procIndex < procNum; procIndex++)
    {
        if ((pid =fork())== 0)
        {
            if(*background == 1)
            {
                if(procIndex == 0)
                {
                    tmpIndex = pid;
                    PROCTABLE[*procCount].pid = pid;
                    strcpy(PROCTABLE[*procCount].name,backgroundPrompt);
                    *procCount = *procCount+1;
                }
            }

            break;
        }
        else if (pid < 0)
        {
            for (int i = 0; i < pipeNum; i++)
            {
                close(pipeFd[i][0]);
                close(pipeFd[i][1]);
            }
            return FORKERR;
        }
    }
    if(procIndex == 0)
    {
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
        redirection_t(pipeRedirect+procIndex);
        cpyArgsExec(procIndex, pipeMark, args,shmid,errorCode);
    }
    else if (procIndex == procNum - 1)
    {
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
        dup2(pipeFd[procIndex - 1][0], STDIN_FILENO);
        redirection_t(pipeRedirect+procIndex);
        cpyArgsExec(procIndex, pipeMark, args,shmid,errorCode);
    }
    else if (procNum > 2 && procIndex > 0 && procIndex < procNum-1)
    {
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
        redirection_t(pipeRedirect+procIndex);
        cpyArgsExec(procIndex, pipeMark, args,shmid,errorCode);
    }
    else
    {

        for (int i = 0; i < pipeNum; i++)
        {
            close(pipeFd[i][0]);
            close(pipeFd[i][1]);
        }
        if(*background == 0)
        {
            while((wpid = waitpid(-1,&tmpLoc,WNOHANG)) != -1)
            {
                if(wpid > 0)
                {
                    count++;
                }

            }
        }
        else if(*background == 1)
        {
            waitpid(tmpIndex,NULL,WNOHANG);

        }
        shmctl(shmid,IPC_STAT,&buf);
        errorCode = (int*) shmat(shmid,NULL,0);
        err = *errorCode;
        shmdt(errorCode);
        shmctl(shmid,IPC_RMID,NULL);

    }

    if((err % 1000) != 0)
    {
        return PIPE_CMD_NOT_FOUND - pipeMark[err/1000];
    }

    return PIPE_END_OF_LINE;

}
