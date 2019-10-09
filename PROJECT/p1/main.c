#include "shell.h"
int main()
{
    glbInit();
    struct sigaction sa;
    sa.sa_handler = sigHandler_C;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    loopProcess();
    glbFree();
    //free(exeError);
    exit(0);
}
