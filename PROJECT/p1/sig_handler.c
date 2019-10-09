#include "shell.h"
void sigHandler_C()
{
    fflush(stdin);
    *ctrlC= SIGINT;
}


