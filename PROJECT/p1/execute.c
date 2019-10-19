#include "shell.h"

void cpyArgsExec(int procIndex,int* pipeMark,char** args,int shmid,int* errorCode)
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
    for(int i = 0 ; i< tmpArgNums+1;i++)
    {
        free(tmpArgs[i]);
    }
    free(tmpArgs);
    errorCode = (int*) shmat(shmid,NULL,0);
    *errorCode = errno + procIndex*1000;
    shmdt(errorCode);
    exit(0);
}

int execute(char **args, int argNum,int inputState)
{
    if(*background == 1)
    {
        for(int i = 0; i< *procCount;i++)
        {
            if(waitpid(PROCTABLE[i].pid,NULL,WNOHANG) == 0)
            {
                PROCTABLE[i].done = 0;
            }
            else
            {
                PROCTABLE[i].done = 1;
            }

        }
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

    int shmid = shmget(IPC_PRIVATE,MAXLINE,IPC_CREAT | 0600);
    struct shmid_ds buf;
    int* errorCode;
    int tmpLoc = 0;
    int err = 0;
    if(!strcmp(args[0],"cd"))
    {
        myCd(args);
    }
    else
    {
        pid_t pid = fork();

        if(*background == 1)
        {
            PROCTABLE[*procCount].pid = pid;
            strcpy(PROCTABLE[*procCount].name,backgroundPrompt);
            *procCount = *procCount+1;
        }

        if(pid<0)
        {
            return FORKERR;
        }

        if(pid == 0)
        {
            if(builtinCommand(args) == 0)
            {
                args[argNum] = NULL;
                execvp(args[0],args);
                errorCode = (int*) shmat(shmid,NULL,0);
                *errorCode = errno;
                shmdt(errorCode);
                exit(0);
            }
            else
            {
                exit(0);
            }
        }
        else
        {
            if(*background == 1)
            {
                waitpid(pid,&tmpLoc,WNOHANG);
            }
            else if (*background == 0)
            {
                waitpid(pid,&tmpLoc,WUNTRACED);
            }

            shmctl(shmid,IPC_STAT,&buf);
            errorCode = (int*) shmat(shmid,NULL,0);
            err = *errorCode;

            shmdt(errorCode);
            shmctl(shmid,IPC_RMID,NULL);
        }
    }

    if(err != 0)
    {
        return COMMAND_NOT_FOUND;
    }
    else
    {
        return inputState;
    }

}
