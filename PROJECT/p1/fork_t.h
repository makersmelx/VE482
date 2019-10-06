
#ifndef P1_FORK_T_H
#define P1_FORK_T_H
#include "shell.h"
typedef struct fork_t
{
    int forkNum;
    int forkPid[MAX_PIPE];
    int sigStatus;
}fork_t;

fork_t *allFork;

#endif //P1_FORK_T_H
