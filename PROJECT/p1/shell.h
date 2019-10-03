#ifndef SHELL_H
#define SHELL_H
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#define MAXLINE 1024
#define MAXARGS 20
#define SINGLE_LEFT_REDIRECTION -1
#define DOUBLE_RIGHT_REDIRECTION 2
#define SINGLE_RIGHT_REDIRECTION 1
#define PROCESSING 0
#define END_OF_LINE 10
#define PIPE 11
#define EXIT 482477
#define MAX_PIPE 20

typedef struct fork_t
{
    int forkNum;
    int forkPid[MAX_PIPE];
    int sigStatus;
}fork_t;

void process();

void print_prompt();
int read_command(char **, int *);

void execute(char **, int);
int redirection(int, char*);

int builtinCommand(char**);

void myCd(char**);
void myPwd();

void sigHandler();
void addToAllFork(int);
void resetFork();


fork_t *allFork;

#endif
