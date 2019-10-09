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
#include <sys/shm.h>
#include <sys/ipc.h>
/*=====================*
      User config
 *=====================*/
#include "error_code.h"
#include "max_limits.h"
#include "running_code.h"
#include "redirect_t.h"
#include "proc.h"
int* ctrlC;
int* background;
char* backgroundPrompt;
int* command;
int *procCount;
int *procCount_storage;
int *crossLine;

typedef struct FilePath
{
    char path[MAXLINE];
    int state;
    int used;
}gl_fp;

redirect_t tmpfp;
myProc PROCTABLE[MAX_PIPE+1];
//int* exeError;
//#define DEBUG

/*======================================*
               processing.c
 *======================================*/
void glbInit();
void glbFree();
void errorFlush();
int process(int,char**,char*,int*,int,redirect_t*,int,int*,int*);
void loopProcess();
/*======================================*
               print_prompt.c
 *======================================*/
void print_prompt(int*);
void printExit();
void prompt_any(char*);
void errorPrompt(int,char*);

/*======================================*
               read_command.c
 *======================================*/
void catToBGPrompt(char*);
void strclear(char*,int);
int commaHandle(char,char*);
int read_command(char **, int *);
/*======================================*
               execute.c
 *======================================*/
int execute(char **, int,int);
void cpyArgsExec(int,int*,char**,int,int*);

/*======================================*
               redirection.c
 *======================================*/
int redirection(int, char*,int,redirect_t*,int,int*,int*);
void redirection_t(redirect_t*);
/*======================================*
               builtin_command.c
 *======================================*/
int builtinCommand(char**);
void myPwd();

/*======================================*
               cd.c
 *======================================*/
void myCd(char**);

/*======================================*
               sigHandler.c
 *======================================*/
void sigHandler_C();

/*======================================*
               pipe.c
 *======================================*/
int myPipe(int**,int*,char**,int,redirect_t*);
#ifdef DEBUG
void pipeTest(redirect_t, int, int);
#endif


#endif
