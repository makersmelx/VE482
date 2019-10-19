
#ifndef P1_PROC_H
#define P1_PROC_H

typedef struct myStruct
{
    int pid;
    int done;
    char name[MAXLINE];
}myProc;

myProc PROCTABLE[MAX_PIPE+1];

int *procCount;

#endif //P1_PROC_H
