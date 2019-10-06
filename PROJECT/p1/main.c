#include "shell.h"
int main()
{
    allFork = malloc(sizeof(fork_t));
    struct sigaction sa;
    sa.sa_handler = sigHandler_C;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    resetFork();
    loopProcess();
}
