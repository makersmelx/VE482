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
/*=====================*
      User config
 *=====================*/
#include "error_code.h"
#include "max_limits.h"
#include "running_code.h"
#include "fork_t.h"
#include "redirect_t.h"

//#define DEBUG

/*======================================*
               processing.c
 *======================================*/
int process(int,char**,char*,int*,int,redirect_t*,int);
void loopProcess();
/*======================================*
               print_prompt.c
 *======================================*/
void print_prompt(int*);
void printExit();
void errorPrompt();

/*======================================*
               read_command.c
 *======================================*/
int read_command(char **, int *);

/*======================================*
               execute.c
 *======================================*/
void execute(char **, int);
void cpyArgsExec(int,int*,char**);

/*======================================*
               redirection.c
 *======================================*/
int redirection(int, char*,int,redirect_t*,int);
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
void sigHandler_D();

/*======================================*
               pid_Manage.c
 *======================================*/
void addToAllFork(int);
void resetFork();

/*======================================*
               pipe.c
 *======================================*/
int myPipe(int**,int*,char**,int,redirect_t*);
#ifdef DEBUG
void pipeTest(redirect_t, int, int);
#endif


#endif
