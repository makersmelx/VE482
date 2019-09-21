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
#define MAXLINE 1024
#define MAXARGS 20
#define SINGLE_LEFT_REDIRECTION -1
#define DOUBLE_RIGHT_REDIRECTION 2
#define SINGLE_RIGHT_REDIRECTION 1
#define PROCESSING 0
#define END_OF_LINE 10
#define PIPE 11
#define EXIT 482477

void print_prompt();
int read_command(char **, int *);

void execute(char **, int);
int redirection(int, char*);

#endif
